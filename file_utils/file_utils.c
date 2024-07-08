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
#define ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE 0x06064b50
#define ZIP_LOCATOR_SIGNATURE 0x07064b50

#define LOCAL_FILE_HEADER_SIZE 30
#define CENTRAL_DIRECTORY_HEADER_SIZE 46
#define ZIP_LOCATOR_SIZE 20
#define ZIP_DIRECTORY_END_SIZE 22
#define ZIP_END_OF_CENTRAL_DIRECTORY 56

#define READ_FILE_BUFFER (1024 * 16)

error get_file_size(const char *filename, uint64_t* file_size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return FAIL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return FAIL;
    }

    uint64_t fileSize = ftell(file);
    if (fileSize == -1) {
        return FAIL;
    }

    *file_size = fileSize;

    fclose(file);
    return SUCCESS;
}

typedef struct {
    uint16_t signature;
    uint16_t size_extra_field;
    uint64_t uncompressed_size;
    uint64_t compressed_size;
} extra_field_t;

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
    extra_field_t extra_field;
} central_directory_header_t;

typedef struct {
    uint32_t signature;
    uint64_t size_of_eocd_minus_12;
    uint16_t version_made_by;
    uint16_t version_needed;
    uint32_t number_on_disk;
    uint32_t disk_where_central_directory_starts;
    uint64_t number_of_central_directories_on_disk;
    uint64_t number_of_central_directories;
    uint64_t size_of_cental_directory;
    uint64_t offset_of_start_of_central_directory;
}   end_of_central_directory_record_t;

error write_local_file_header(socket_t *socket, const char* filename, uint32_t crc32) {
    uint8_t local_file_header[LOCAL_FILE_HEADER_SIZE] = {0};
    *(uint32_t*)local_file_header = ZIP_LOCAL_FILE_HEADER_SIGNATURE;    // signature
    *(uint16_t*)(local_file_header + 4) = 20;                           // version needed
    *(uint16_t*)(local_file_header + 6) = 0;                            // general purpose bit flag
    *(uint16_t*)(local_file_header + 8) = 0;                            // compression method
    *(uint16_t*)(local_file_header + 10) = 0;                           // last modification time
    *(uint16_t*)(local_file_header + 12) = 0;                           // las modification date
    *(uint32_t*)(local_file_header + 14) = crc32;                           // crc32
    *(uint32_t*)(local_file_header + 18) = 0;                           // compressed file size
    *(uint32_t*)(local_file_header + 22) = 0;                           // uncompressed file size
    *(uint16_t*)(local_file_header + 26) = strlen(filename);         // file name length
    *(uint16_t*)(local_file_header + 28) = 0;                           // extra field length

    socket_t__write(socket, local_file_header, LOCAL_FILE_HEADER_SIZE, NULL);
    socket_t__write(socket, filename, strlen(filename), NULL);
    FILE* f = fopen(filename, "rb");
    if(f == NULL){
        return FAIL;
    }

    char buffer[READ_FILE_BUFFER];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, READ_FILE_BUFFER, f)) > 0){
        socket_t__write(socket, buffer, bytes_read, NULL);
    }

    fclose(f);
    return SUCCESS;
}

void write_central_directory_header(socket_t* socket, central_directory_header_t * header, char* filename) {
    socket_t__write(socket, &header->signature, sizeof(header->signature), NULL);
    socket_t__write(socket, &header->version_made_by, sizeof(header->version_made_by), NULL);
    socket_t__write(socket, &header->version_needed, sizeof(header->version_needed), NULL);
    socket_t__write(socket, &header->general_purpose_bit_flag, sizeof(header->general_purpose_bit_flag), NULL);
    socket_t__write(socket, &header->compression_method, sizeof(header->compression_method), NULL);
    socket_t__write(socket, &header->last_mod_file_time, sizeof(header->last_mod_file_time), NULL);
    socket_t__write(socket, &header->last_mod_file_date, sizeof(header->last_mod_file_date), NULL);
    socket_t__write(socket, &header->crc32, sizeof(header->crc32), NULL);
    socket_t__write(socket, &header->compressed_size, sizeof(header->compressed_size), NULL);
    socket_t__write(socket, &header->uncompressed_size, sizeof(header->uncompressed_size), NULL);
    socket_t__write(socket, &header->file_name_length, sizeof(header->file_name_length), NULL);
    socket_t__write(socket, &header->extra_field_length, sizeof(header->extra_field_length), NULL);
    socket_t__write(socket, &header->file_comment_length, sizeof(header->file_comment_length), NULL);
    socket_t__write(socket, &header->disk_number_start, sizeof(header->disk_number_start), NULL);
    socket_t__write(socket, &header->internal_file_attributes, sizeof(header->internal_file_attributes), NULL);
    socket_t__write(socket, &header->external_file_attributes, sizeof(header->external_file_attributes), NULL);
    socket_t__write(socket, &header->relative_offset_of_local_header, sizeof(header->relative_offset_of_local_header),NULL);

    socket_t__write(socket, filename, header->file_name_length, NULL);

    socket_t__write(socket, &header->extra_field.signature, sizeof(header->extra_field.signature), NULL);
    socket_t__write(socket, &header->extra_field.size_extra_field, sizeof(header->extra_field.size_extra_field), NULL);
    socket_t__write(socket, &header->extra_field.uncompressed_size, sizeof(header->extra_field.uncompressed_size), NULL);
    socket_t__write(socket, &header->extra_field.compressed_size, sizeof(header->extra_field.compressed_size), NULL);
}

void write_end_of_central_directory_record(socket_t* socket, end_of_central_directory_record_t* eocd) {
    socket_t__write(socket, &eocd->signature, sizeof(eocd->signature), NULL);
    socket_t__write(socket, &eocd->size_of_eocd_minus_12, sizeof(eocd->size_of_eocd_minus_12), NULL);
    socket_t__write(socket, &eocd->version_made_by, sizeof(eocd->version_made_by), NULL);
    socket_t__write(socket, &eocd->version_needed, sizeof(eocd->version_needed), NULL);
    socket_t__write(socket, &eocd->number_on_disk, sizeof(eocd->number_on_disk), NULL);
    socket_t__write(socket, &eocd->disk_where_central_directory_starts,
                    sizeof(eocd->disk_where_central_directory_starts), NULL);
    socket_t__write(socket, &eocd->number_of_central_directories_on_disk,
                    sizeof(eocd->number_of_central_directories_on_disk), NULL);
    socket_t__write(socket, &eocd->number_of_central_directories, sizeof(eocd->number_of_central_directories), NULL);
    socket_t__write(socket, &eocd->size_of_cental_directory, sizeof(eocd->size_of_cental_directory), NULL);
    socket_t__write(socket, &eocd->offset_of_start_of_central_directory,
                    sizeof(eocd->offset_of_start_of_central_directory), NULL);

}

error m_crc32(const char *filename, uint32_t *crc32) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return FAIL;
    }
    uint32_t crc = 0xe320bbde;
    rewind(file);

    char buffer[READ_FILE_BUFFER];
    size_t bytes_read;
    while((bytes_read = fread(buffer, READ_FILE_BUFFER, 1, file)) > 0) {
        for (uint64_t i=0; i < bytes_read; ++i) {
            crc ^= buffer[i];
            if (crc & 1){
                for (int _ = 0; _ < 8; ++_) {
                    crc = (crc >> 1) ^ 0xEDB88320;
                }
            }
            else {
                for (int _ = 0; _ < 8; ++_) {
                    crc = crc >> 1;
                }
            }
        }
    }

    fclose(file);
    *crc32 = crc ^ 0xe320bbde;
    return SUCCESS;
}

error write_zip_to_socket(list_strings_t* files, socket_t* socket) {
    central_directory_header_t* central_directory_headers = calloc(files->size, CENTRAL_DIRECTORY_HEADER_SIZE);
    if(central_directory_headers == NULL){
        return FAIL;
    }

    uint64_t central_directory_offset = 0;
    uint64_t central_directory_size = 0;
    for (int i = 0; i < files->size; i++) {
        char *filename;
        error err = list_strings_t__get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }

        uint64_t file_size;
        err = get_file_size(filename, &file_size);
        if(err != SUCCESS){
            return err;
        }

        uint32_t crc = 0;
        err = m_crc32(filename, &crc);
        if(err != SUCCESS){
            return err;
        }

        write_local_file_header(socket, filename, crc);

        central_directory_headers[i].signature = ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE;
        central_directory_headers[i].version_made_by = 20;
        central_directory_headers[i].version_needed = 45;
        central_directory_headers[i].general_purpose_bit_flag = 0;
        central_directory_headers[i].compression_method = 0;  // STORE
        central_directory_headers[i].last_mod_file_time = 0;
        central_directory_headers[i].last_mod_file_date = 0;
        central_directory_headers[i].crc32 = crc;
        central_directory_headers[i].compressed_size = 0xFFFFFFFF;
        central_directory_headers[i].uncompressed_size = 0xFFFFFFFF;
        central_directory_headers[i].file_name_length = strlen(filename);
        central_directory_headers[i].extra_field_length = 20;
        central_directory_headers[i].file_comment_length = 0;
        central_directory_headers[i].disk_number_start = 0;
        central_directory_headers[i].internal_file_attributes = 0;
        central_directory_headers[i].external_file_attributes = 0;
        central_directory_headers[i].relative_offset_of_local_header = central_directory_offset;
        central_directory_headers[i].extra_field.signature = 0x0001;
        central_directory_headers[i].extra_field.size_extra_field = 16;
        central_directory_headers[i].extra_field.compressed_size = file_size;
        central_directory_headers[i].extra_field.uncompressed_size = file_size;

        central_directory_offset += LOCAL_FILE_HEADER_SIZE + strlen(filename) + file_size;;
        central_directory_size += CENTRAL_DIRECTORY_HEADER_SIZE + central_directory_headers[i].extra_field_length + strlen(filename);
    }

    for (int i = 0; i < files->size; i++) {
        char *filename;
        error err = list_strings_t__get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }

        write_central_directory_header(socket, &central_directory_headers[i], filename);
    }

    char buffer[ZIP_END_OF_CENTRAL_DIRECTORY];
    *(uint32_t *)(buffer) = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE; // signature
    *(uint64_t *)(buffer + 4) = 44; // size_of_eocd_minus_12
    *(uint16_t *)(buffer + 12) = 45; // version_made_by
    *(uint16_t *)(buffer + 14) = 45; // version_needed
    *(uint32_t *)(buffer + 16) = 0; // number_on_disk
    *(uint32_t *)(buffer + 20) = 0; // disk_where_central_directory_starts
    *(uint64_t *)(buffer + 24) = files->size; // number_of_central_directories_on_disk
    *(uint64_t *)(buffer + 32) = files->size; // number_of_central_directories
    *(uint64_t *)(buffer + 40) = central_directory_size; // size_of_cental_directory
    *(uint64_t *)(buffer + 48) = central_directory_offset; // offset_of_start_of_central_directory
    socket_t__write(socket, buffer, ZIP_END_OF_CENTRAL_DIRECTORY, NULL);

    char zip_locator_buffer[ZIP_LOCATOR_SIZE];
    *(uint32_t *)(zip_locator_buffer) = ZIP_LOCATOR_SIGNATURE; // signature
    *(uint32_t *)(zip_locator_buffer + 4) = 0; // number of the disk with the start of the zip64 end of central directory
    *(uint64_t *)(zip_locator_buffer + 8) = central_directory_offset + central_directory_size; // relative offset of the zip64 end of central directory record
    *(uint32_t *)(zip_locator_buffer + 16) = 1; // total number of disks
    socket_t__write(socket, zip_locator_buffer, ZIP_LOCATOR_SIZE, NULL);

    char end_of_record_buffer[ZIP_DIRECTORY_END_SIZE];
    *(uint32_t *)(end_of_record_buffer) = 101010256; // directoryEndSignature
    *(uint16_t *)(end_of_record_buffer + 4) = 0;
    *(uint16_t *)(end_of_record_buffer + 6) = 0;
    *(uint16_t *)(end_of_record_buffer + 8) = 0xFFFF; // number of entries this disk
    *(uint16_t *)(end_of_record_buffer + 10) = 0xFFFF; // number of entries total
    *(uint32_t *)(end_of_record_buffer + 12) = 0xFFFFFFFF; // size of directory
    *(uint32_t *)(end_of_record_buffer + 16) = 0xFFFFFFFF; // start of directory
    *(uint16_t *)(end_of_record_buffer + 20) = 0; // byte size of eocd command
    socket_t__write(socket, end_of_record_buffer, ZIP_DIRECTORY_END_SIZE, NULL);

    return SUCCESS;
}