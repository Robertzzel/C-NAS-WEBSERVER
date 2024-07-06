//
// Created by robert on 7/5/24.
//

#include <string.h>
#include "file_utils.h"
#include "../error.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 ZIP FILE STRUCTURE:
    LOCAL_FILE_HEADER1
    FILE_CONTENT1
    LOCAL_FILE_HEADER2
    FILE_CONTENT2
    ...
    CENTRAL_DIRECTORY_HEADER1
    CENTRAL_DIRECTORY_HEADER2
    ...
    END OF CENTRAL DIRECTORY HEADER
 */

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE 0x02014b50
#define ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE 0x06054b50
#define LOCAL_FILE_HEADER_SIZE 30
#define CENTRAL_DIRECTORY_HEADER_SIZE 46

error get_file_size(const char *filename, uint32_t* file_size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return FAIL;
    }

    // Seek to the end of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        return FAIL;
    }

    // Get the current position of the file pointer, which is the file size
    uint32_t fileSize = ftell(file);
    if (fileSize == -1) {
        return FAIL;
    }

    *file_size = fileSize;

    fclose(file);
    return SUCCESS;
}

typedef struct {
    uint32_t signature;
    uint16_t version_needed;
    uint16_t general_purpose_bit_flag;
    uint16_t compression_method;
    uint16_t last_mod_file_time;
    uint16_t last_mod_file_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t file_name_length;
    uint16_t extra_field_length;
} local_file_header_t;

typedef struct {
    uint32_t signature;
    uint16_t version_made_by;
    uint16_t version_needed;
    uint16_t general_purpose_bit_flag;
    uint16_t compression_method;
    uint16_t last_mod_file_time;
    uint16_t last_mod_file_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t file_name_length;
    uint16_t extra_field_length;
    uint16_t file_comment_length;
    uint16_t disk_number_start;
    uint16_t internal_file_attributes;
    uint32_t external_file_attributes;
    uint32_t relative_offset_of_local_header;
} central_directory_header_t;

typedef struct {
    uint32_t signature;
    uint16_t number_of_this_disk;
    uint16_t number_of_the_disk_with_the_start_of_the_central_directory;
    uint16_t total_number_of_entries_in_the_central_directory_on_this_disk;
    uint16_t total_number_of_entries_in_the_central_directory;
    uint32_t size_of_the_central_directory;
    uint32_t offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number;
    uint16_t zip_file_comment_length;
} end_of_central_directory_record_t;
#pragma pack(pop)

error m_crc32(const char *filename, uint32_t *crc32) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return FAIL;
    }
    uint32_t crc = 0xFFFFFFFF;

    // Seek to the end of the file to determine its size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Read the file backwards by iterating from end to start
    for (long i = fileSize - 1; i >= 0; i--) {
        int ch = fgetc(file);
        // Print or process the character (you can modify this part)
        crc ^= ch;
        for (int _ = 0; _ < 8; ++_) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }

    fclose(file);
    *crc32 = crc ^ 0xFFFFFFFF;
    return SUCCESS;
}

error write_zip_file(const char *zip_filename, array_of_strings_t* files) {
    FILE *zip_file = fopen(zip_filename, "wb");
    if (!zip_file) {
        return FAIL;
    }

    local_file_header_t* local_files_headers = calloc(files->size, sizeof(local_file_header_t));
    if(local_files_headers == NULL){
        return FAIL;
    }
    central_directory_header_t* central_directory_headers = calloc(files->size, sizeof(central_directory_header_t));
    if(central_directory_headers == NULL){
        return FAIL;
    }

    unsigned int central_directory_offset = 0;
    unsigned int central_directory_size = 0;
    for (int i = 0; i < files->size; i++) {
        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }

        uint32_t file_size;
        err = get_file_size(filename, &file_size);
        if(err != SUCCESS){
            return err;
        }

        uint32_t crc = 0;
        err = m_crc32(filename, &crc);
        if(err != SUCCESS){
            return err;
        }

        local_files_headers[i].signature = ZIP_LOCAL_FILE_HEADER_SIGNATURE;
        local_files_headers[i].version_needed = 20;
        local_files_headers[i].general_purpose_bit_flag = 0;
        local_files_headers[i].compression_method = 0;  // STORE
        local_files_headers[i].last_mod_file_time = 0;
        local_files_headers[i].last_mod_file_date = 0;
        local_files_headers[i].crc32 = crc;
        local_files_headers[i].compressed_size = file_size;
        local_files_headers[i].uncompressed_size = file_size;
        local_files_headers[i].file_name_length = strlen(filename);
        local_files_headers[i].extra_field_length = 0;

        central_directory_headers[i].signature = ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE;
        central_directory_headers[i].version_made_by = 20;
        central_directory_headers[i].version_needed = 20;
        central_directory_headers[i].compression_method = 0;  // STORE
        central_directory_headers[i].last_mod_file_time = 0;
        central_directory_headers[i].last_mod_file_date = 0;
        central_directory_headers[i].crc32 = crc;
        central_directory_headers[i].compressed_size = file_size;
        central_directory_headers[i].uncompressed_size = file_size;
        central_directory_headers[i].file_name_length = strlen(filename);
        central_directory_headers[i].relative_offset_of_local_header = central_directory_offset;

        central_directory_offset += LOCAL_FILE_HEADER_SIZE + strlen(filename) + file_size;
        central_directory_size += CENTRAL_DIRECTORY_HEADER_SIZE + strlen(filename);
    }

    for (int i = 0; i < files->size; i++) {
        fwrite(&local_files_headers[i].signature, sizeof(local_files_headers[i].signature), 1, zip_file);
        fwrite(&local_files_headers[i].version_needed, sizeof(local_files_headers[i].version_needed), 1, zip_file);
        fwrite(&local_files_headers[i].general_purpose_bit_flag, sizeof(local_files_headers[i].general_purpose_bit_flag), 1, zip_file);
        fwrite(&local_files_headers[i].compression_method, sizeof(local_files_headers[i].compression_method), 1, zip_file);
        fwrite(&local_files_headers[i].last_mod_file_time, sizeof(local_files_headers[i].last_mod_file_time), 1, zip_file);
        fwrite(&local_files_headers[i].last_mod_file_date, sizeof(local_files_headers[i].last_mod_file_date), 1, zip_file);
        fwrite(&local_files_headers[i].crc32, sizeof(local_files_headers[i].crc32), 1, zip_file);
        fwrite(&local_files_headers[i].compressed_size, sizeof(local_files_headers[i].compressed_size), 1, zip_file);
        fwrite(&local_files_headers[i].uncompressed_size, sizeof(local_files_headers[i].uncompressed_size), 1, zip_file);
        fwrite(&local_files_headers[i].file_name_length, sizeof(local_files_headers[i].file_name_length), 1, zip_file);
        fwrite(&local_files_headers[i].extra_field_length, sizeof(local_files_headers[i].extra_field_length), 1, zip_file);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        fwrite(filename, local_files_headers[i].file_name_length, 1, zip_file);

        FILE* f = fopen(filename, "rb");
        if(f == NULL){
            return FAIL;
        }
        char buffer[1024];
        size_t bytes_read = fread(buffer, 1, 1024, f);
        while(bytes_read != 0){
            fwrite(buffer, bytes_read, 1, zip_file);
            bytes_read = fread(buffer, 1, 1024, f);
        }
        fclose(f);
    }

    for (int i = 0; i < files->size; i++) {
        fwrite(&central_directory_headers[i].signature, sizeof(central_directory_headers[i].signature), 1, zip_file);
        fwrite(&central_directory_headers[i].version_made_by, sizeof(central_directory_headers[i].version_made_by), 1, zip_file);
        fwrite(&central_directory_headers[i].version_needed, sizeof(central_directory_headers[i].version_needed), 1, zip_file);
        fwrite(&central_directory_headers[i].general_purpose_bit_flag, sizeof(central_directory_headers[i].general_purpose_bit_flag), 1, zip_file);
        fwrite(&central_directory_headers[i].compression_method, sizeof(central_directory_headers[i].compression_method), 1, zip_file);
        fwrite(&central_directory_headers[i].last_mod_file_time, sizeof(central_directory_headers[i].last_mod_file_time), 1, zip_file);
        fwrite(&central_directory_headers[i].last_mod_file_date, sizeof(central_directory_headers[i].last_mod_file_date), 1, zip_file);
        fwrite(&central_directory_headers[i].crc32, sizeof(central_directory_headers[i].crc32), 1, zip_file);
        fwrite(&central_directory_headers[i].compressed_size, sizeof(central_directory_headers[i].compressed_size), 1, zip_file);
        fwrite(&central_directory_headers[i].uncompressed_size, sizeof(central_directory_headers[i].uncompressed_size), 1, zip_file);
        fwrite(&central_directory_headers[i].file_name_length, sizeof(central_directory_headers[i].file_name_length), 1, zip_file);
        fwrite(&central_directory_headers[i].extra_field_length, sizeof(central_directory_headers[i].extra_field_length), 1, zip_file);
        fwrite(&central_directory_headers[i].file_comment_length, sizeof(central_directory_headers[i].file_comment_length), 1, zip_file);
        fwrite(&central_directory_headers[i].disk_number_start, sizeof(central_directory_headers[i].disk_number_start), 1, zip_file);
        fwrite(&central_directory_headers[i].internal_file_attributes, sizeof(central_directory_headers[i].internal_file_attributes), 1, zip_file);
        fwrite(&central_directory_headers[i].external_file_attributes, sizeof(central_directory_headers[i].external_file_attributes), 1, zip_file);
        fwrite(&central_directory_headers[i].relative_offset_of_local_header, sizeof(central_directory_headers[i].relative_offset_of_local_header), 1, zip_file);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        fwrite(filename, central_directory_headers[i].file_name_length, 1, zip_file);
    }

    end_of_central_directory_record_t eocd = {0};
    eocd.signature = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE;
    eocd.total_number_of_entries_in_the_central_directory_on_this_disk = files->size;
    eocd.total_number_of_entries_in_the_central_directory = files->size;
    eocd.size_of_the_central_directory = central_directory_size;
    eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number = central_directory_offset;

    fwrite(&eocd.signature, sizeof(eocd.signature), 1, zip_file);
    fwrite(&eocd.number_of_this_disk, sizeof(eocd.number_of_this_disk), 1, zip_file);
    fwrite(&eocd.number_of_the_disk_with_the_start_of_the_central_directory, sizeof(eocd.number_of_the_disk_with_the_start_of_the_central_directory), 1, zip_file);
    fwrite(&eocd.total_number_of_entries_in_the_central_directory_on_this_disk, sizeof(eocd.total_number_of_entries_in_the_central_directory_on_this_disk), 1, zip_file);
    fwrite(&eocd.total_number_of_entries_in_the_central_directory, sizeof(eocd.total_number_of_entries_in_the_central_directory), 1, zip_file);
    fwrite(&eocd.size_of_the_central_directory, sizeof(eocd.size_of_the_central_directory), 1, zip_file);
    fwrite(&eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number, sizeof(eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number), 1, zip_file);
    fwrite(&eocd.zip_file_comment_length, sizeof(eocd.zip_file_comment_length), 1, zip_file);

    fclose(zip_file);

    return SUCCESS;
}

error write_zip_to_socket(array_of_strings_t* files, s_socket* socket) {
    local_file_header_t* local_files_headers = calloc(files->size, sizeof(local_file_header_t));
    if(local_files_headers == NULL){
        return FAIL;
    }
    central_directory_header_t* central_directory_headers = calloc(files->size, sizeof(central_directory_header_t));
    if(central_directory_headers == NULL){
        return FAIL;
    }

    unsigned int central_directory_offset = 0;
    unsigned int central_directory_size = 0;
    for (int i = 0; i < files->size; i++) {
        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }

        uint32_t file_size;
        err = get_file_size(filename, &file_size);
        if(err != SUCCESS){
            return err;
        }

        uint32_t crc = 0;
        err = m_crc32(filename, &crc);
        if(err != SUCCESS){
            return err;
        }

        local_files_headers[i].signature = ZIP_LOCAL_FILE_HEADER_SIGNATURE;
        local_files_headers[i].version_needed = 20;
        local_files_headers[i].general_purpose_bit_flag = 0;
        local_files_headers[i].compression_method = 0;  // STORE
        local_files_headers[i].last_mod_file_time = 0;
        local_files_headers[i].last_mod_file_date = 0;
        local_files_headers[i].crc32 = crc;
        local_files_headers[i].compressed_size = file_size;
        local_files_headers[i].uncompressed_size = file_size;
        local_files_headers[i].file_name_length = strlen(filename);
        local_files_headers[i].extra_field_length = 0;

        central_directory_headers[i].signature = ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE;
        central_directory_headers[i].version_made_by = 20;
        central_directory_headers[i].version_needed = 20;
        central_directory_headers[i].compression_method = 0;  // STORE
        central_directory_headers[i].last_mod_file_time = 0;
        central_directory_headers[i].last_mod_file_date = 0;
        central_directory_headers[i].crc32 = crc;
        central_directory_headers[i].compressed_size = file_size;
        central_directory_headers[i].uncompressed_size = file_size;
        central_directory_headers[i].file_name_length = strlen(filename);
        central_directory_headers[i].relative_offset_of_local_header = central_directory_offset;

        central_directory_offset += LOCAL_FILE_HEADER_SIZE + strlen(filename) + file_size;
        central_directory_size += CENTRAL_DIRECTORY_HEADER_SIZE + strlen(filename);
    }

    for (int i = 0; i < files->size; i++) {
        socket_write(socket, &local_files_headers[i].signature, sizeof(local_files_headers[i].signature), NULL);
        socket_write(socket, &local_files_headers[i].version_needed, sizeof(local_files_headers[i].version_needed), NULL);
        socket_write(socket, &local_files_headers[i].general_purpose_bit_flag, sizeof(local_files_headers[i].general_purpose_bit_flag), NULL);
        socket_write(socket, &local_files_headers[i].compression_method, sizeof(local_files_headers[i].compression_method), NULL);
        socket_write(socket, &local_files_headers[i].last_mod_file_time, sizeof(local_files_headers[i].last_mod_file_time), NULL);
        socket_write(socket, &local_files_headers[i].last_mod_file_date, sizeof(local_files_headers[i].last_mod_file_date), NULL);
        socket_write(socket, &local_files_headers[i].crc32, sizeof(local_files_headers[i].crc32), NULL);
        socket_write(socket, &local_files_headers[i].compressed_size, sizeof(local_files_headers[i].compressed_size), NULL);
        socket_write(socket, &local_files_headers[i].uncompressed_size, sizeof(local_files_headers[i].uncompressed_size), NULL);
        socket_write(socket, &local_files_headers[i].file_name_length, sizeof(local_files_headers[i].file_name_length), NULL);
        socket_write(socket, &local_files_headers[i].extra_field_length, sizeof(local_files_headers[i].extra_field_length), NULL);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        socket_write(socket, filename, local_files_headers[i].file_name_length, NULL);

        FILE* f = fopen(filename, "rb");
        if(f == NULL){
            return FAIL;
        }
        char buffer[1024];
        size_t bytes_read = fread(buffer, 1, 1024, f);
        while(bytes_read != 0){
            socket_write(socket, buffer, bytes_read, NULL);
            bytes_read = fread(buffer, 1, 1024, f);
        }
        fclose(f);
    }

    for (int i = 0; i < files->size; i++) {
        socket_write(socket, &central_directory_headers[i].signature, sizeof(central_directory_headers[i].signature), NULL);
        socket_write(socket, &central_directory_headers[i].version_made_by, sizeof(central_directory_headers[i].version_made_by), NULL);
        socket_write(socket, &central_directory_headers[i].version_needed, sizeof(central_directory_headers[i].version_needed), NULL);
        socket_write(socket, &central_directory_headers[i].general_purpose_bit_flag, sizeof(central_directory_headers[i].general_purpose_bit_flag),NULL);
        socket_write(socket, &central_directory_headers[i].compression_method, sizeof(central_directory_headers[i].compression_method), NULL);
        socket_write(socket, &central_directory_headers[i].last_mod_file_time, sizeof(central_directory_headers[i].last_mod_file_time), NULL);
        socket_write(socket, &central_directory_headers[i].last_mod_file_date, sizeof(central_directory_headers[i].last_mod_file_date), NULL);
        socket_write(socket, &central_directory_headers[i].crc32, sizeof(central_directory_headers[i].crc32), NULL);
        socket_write(socket, &central_directory_headers[i].compressed_size, sizeof(central_directory_headers[i].compressed_size), NULL);
        socket_write(socket, &central_directory_headers[i].uncompressed_size, sizeof(central_directory_headers[i].uncompressed_size), NULL);
        socket_write(socket, &central_directory_headers[i].file_name_length, sizeof(central_directory_headers[i].file_name_length), NULL);
        socket_write(socket, &central_directory_headers[i].extra_field_length, sizeof(central_directory_headers[i].extra_field_length), NULL);
        socket_write(socket, &central_directory_headers[i].file_comment_length, sizeof(central_directory_headers[i].file_comment_length), NULL);
        socket_write(socket, &central_directory_headers[i].disk_number_start, sizeof(central_directory_headers[i].disk_number_start), NULL);
        socket_write(socket, &central_directory_headers[i].internal_file_attributes, sizeof(central_directory_headers[i].internal_file_attributes), NULL);
        socket_write(socket, &central_directory_headers[i].external_file_attributes, sizeof(central_directory_headers[i].external_file_attributes), NULL);
        socket_write(socket, &central_directory_headers[i].relative_offset_of_local_header, sizeof(central_directory_headers[i].relative_offset_of_local_header), NULL);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        socket_write(socket, filename, central_directory_headers[i].file_name_length, NULL);
    }

    end_of_central_directory_record_t eocd = {0};
    eocd.signature = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE;
    eocd.total_number_of_entries_in_the_central_directory_on_this_disk = files->size;
    eocd.total_number_of_entries_in_the_central_directory = files->size;
    eocd.size_of_the_central_directory = central_directory_size;
    eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number = central_directory_offset;

    socket_write(socket, &eocd.signature, sizeof(eocd.signature), NULL);
    socket_write(socket, &eocd.number_of_this_disk, sizeof(eocd.number_of_this_disk), NULL);
    socket_write(socket, &eocd.number_of_the_disk_with_the_start_of_the_central_directory, sizeof(eocd.number_of_the_disk_with_the_start_of_the_central_directory), NULL);
    socket_write(socket, &eocd.total_number_of_entries_in_the_central_directory_on_this_disk, sizeof(eocd.total_number_of_entries_in_the_central_directory_on_this_disk), NULL);
    socket_write(socket, &eocd.total_number_of_entries_in_the_central_directory, sizeof(eocd.total_number_of_entries_in_the_central_directory), NULL);
    socket_write(socket, &eocd.size_of_the_central_directory, sizeof(eocd.size_of_the_central_directory), NULL);
    socket_write(socket, &eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number, sizeof(eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number), NULL);
    socket_write(socket, &eocd.zip_file_comment_length, sizeof(eocd.zip_file_comment_length), NULL);

    return SUCCESS;
}