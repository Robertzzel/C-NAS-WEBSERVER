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
#define DATA_DESCRIPTOR_SIGNATURE 134695760

#define LOCAL_FILE_HEADER_SIZE 30
#define CENTRAL_DIRECTORY_HEADER_SIZE 46

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
} local_file_header_t; // 30 bytes

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
    uint16_t disk_number_start; // astea le sare
    uint16_t internal_file_attributes; //
    uint32_t external_file_attributes;
    uint32_t relative_offset_of_local_header;
} central_directory_header_t; // 46

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
}   end_of_central_directory_record_t; // 56

typedef struct {
    uint16_t signature;
    uint16_t size_extra_field;
    uint64_t uncompressed_size;
    uint64_t compressed_size;
    //uint64_t offset;
} extra_field_t;
#pragma pack(pop)

uint64_t offset_deducted = 0;

void write_local_file_header(s_socket* socket, local_file_header_t* header) {
    socket_write(socket, &header->signature, sizeof(header->signature), NULL);
    offset_deducted += sizeof(header->signature);
    socket_write(socket, &header->version_needed, sizeof(header->version_needed), NULL);
    offset_deducted += sizeof(header->version_needed);
    socket_write(socket, &header->general_purpose_bit_flag, sizeof(header->general_purpose_bit_flag), NULL);
    offset_deducted += sizeof(header->general_purpose_bit_flag);
    socket_write(socket, &header->compression_method, sizeof(header->compression_method), NULL);
    offset_deducted +=sizeof(header->compression_method);
    socket_write(socket, &header->last_mod_file_time, sizeof(header->last_mod_file_time), NULL);
    offset_deducted +=sizeof(header->last_mod_file_time);
    socket_write(socket, &header->last_mod_file_date, sizeof(header->last_mod_file_date), NULL);
    offset_deducted +=sizeof(header->last_mod_file_date);
    socket_write(socket, &header->crc32, sizeof(header->crc32), NULL); // TODO 0
    offset_deducted +=sizeof(header->crc32);
    socket_write(socket, &header->compressed_size, sizeof(header->compressed_size), NULL); // TODO 0
    offset_deducted +=sizeof(header->compressed_size);
    socket_write(socket, &header->uncompressed_size, sizeof(header->uncompressed_size), NULL); // TODO 0
    offset_deducted +=sizeof(header->uncompressed_size);
    socket_write(socket, &header->file_name_length, sizeof(header->file_name_length), NULL);
    offset_deducted +=sizeof(header->file_name_length);
    socket_write(socket, &header->extra_field_length, sizeof(header->extra_field_length), NULL);
    offset_deducted +=sizeof(header->extra_field_length);
}

void write_central_directory_header(s_socket* socket, central_directory_header_t * header) {
    socket_write(socket, &header->signature, sizeof(header->signature), NULL);
    offset_deducted +=sizeof(header->signature);
    socket_write(socket, &header->version_made_by, sizeof(header->version_made_by), NULL);
    offset_deducted +=sizeof(header->version_made_by);
    socket_write(socket, &header->version_needed, sizeof(header->version_needed), NULL);
    offset_deducted +=sizeof(header->version_needed);
    socket_write(socket, &header->general_purpose_bit_flag, sizeof(header->general_purpose_bit_flag),NULL);
    offset_deducted +=sizeof(header->general_purpose_bit_flag);
    socket_write(socket, &header->compression_method, sizeof(header->compression_method), NULL);
    offset_deducted +=sizeof(header->compression_method);
    socket_write(socket, &header->last_mod_file_time, sizeof(header->last_mod_file_time), NULL);
    offset_deducted +=sizeof(header->last_mod_file_time);
    socket_write(socket, &header->last_mod_file_date, sizeof(header->last_mod_file_date), NULL);
    offset_deducted +=sizeof(header->last_mod_file_date);
    socket_write(socket, &header->crc32, sizeof(header->crc32), NULL);
    offset_deducted +=sizeof(header->crc32);
    socket_write(socket, &header->compressed_size, sizeof(header->compressed_size), NULL);
    offset_deducted +=sizeof(header->compressed_size);
    socket_write(socket, &header->uncompressed_size, sizeof(header->uncompressed_size), NULL);
    offset_deducted +=sizeof(header->uncompressed_size);
    socket_write(socket, &header->file_name_length, sizeof(header->file_name_length), NULL);
    offset_deducted +=sizeof(header->file_name_length);
    socket_write(socket, &header->extra_field_length, sizeof(header->extra_field_length), NULL);
    offset_deducted +=sizeof(header->extra_field_length);
    socket_write(socket, &header->file_comment_length, sizeof(header->file_comment_length), NULL);
    offset_deducted +=sizeof(header->file_comment_length);
    socket_write(socket, &header->disk_number_start, sizeof(header->disk_number_start), NULL);
    offset_deducted +=sizeof(header->disk_number_start);
    socket_write(socket, &header->internal_file_attributes, sizeof(header->internal_file_attributes), NULL);
    offset_deducted +=sizeof(header->internal_file_attributes);
    socket_write(socket, &header->external_file_attributes, sizeof(header->external_file_attributes), NULL);
    offset_deducted +=sizeof(header->external_file_attributes);
    socket_write(socket, &header->relative_offset_of_local_header, sizeof(header->relative_offset_of_local_header), NULL);
    offset_deducted +=sizeof(header->relative_offset_of_local_header);
}

void write_end_of_central_directory_record(s_socket* socket, end_of_central_directory_record_t* eocd) {
    socket_write(socket, &eocd->signature, sizeof(eocd->signature), NULL);
    offset_deducted +=sizeof(eocd->signature);

    socket_write(socket, &eocd->size_of_eocd_minus_12, sizeof(eocd->size_of_eocd_minus_12), NULL);
    offset_deducted +=sizeof(eocd->size_of_eocd_minus_12);

    socket_write(socket, &eocd->version_made_by, sizeof(eocd->version_made_by), NULL);
    offset_deducted +=sizeof(eocd->version_made_by);

    socket_write(socket, &eocd->version_needed, sizeof(eocd->version_needed), NULL);
    offset_deducted +=sizeof(eocd->version_needed);

    socket_write(socket, &eocd->number_on_disk, sizeof(eocd->number_on_disk), NULL);
    offset_deducted +=sizeof(eocd->number_on_disk);

    socket_write(socket, &eocd->disk_where_central_directory_starts, sizeof(eocd->disk_where_central_directory_starts), NULL);
    offset_deducted +=sizeof(eocd->disk_where_central_directory_starts);

    socket_write(socket, &eocd->number_of_central_directories_on_disk, sizeof(eocd->number_of_central_directories_on_disk), NULL);
    offset_deducted +=sizeof(eocd->number_of_central_directories_on_disk);

    socket_write(socket, &eocd->number_of_central_directories, sizeof(eocd->number_of_central_directories), NULL);
    offset_deducted +=sizeof(eocd->number_of_central_directories);

    socket_write(socket, &eocd->size_of_cental_directory, sizeof(eocd->size_of_cental_directory), NULL);
    offset_deducted +=sizeof(eocd->size_of_cental_directory);

    socket_write(socket, &eocd->offset_of_start_of_central_directory, sizeof(eocd->offset_of_start_of_central_directory), NULL);
    offset_deducted +=sizeof(eocd->offset_of_start_of_central_directory);
}

void write_extra_field(s_socket* socket, extra_field_t * header) {
    socket_write(socket, &header->signature, sizeof(header->signature), NULL);
    offset_deducted +=sizeof(header->signature);
    socket_write(socket, &header->size_extra_field, sizeof(header->size_extra_field), NULL);
    offset_deducted +=sizeof(header->size_extra_field);
    socket_write(socket, &header->uncompressed_size, sizeof(header->uncompressed_size), NULL);
    offset_deducted +=sizeof(header->uncompressed_size);
    socket_write(socket, &header->compressed_size, sizeof(header->compressed_size), NULL);
    offset_deducted +=sizeof(header->compressed_size);
    //socket_write(socket, &header->offset, sizeof(header->offset), NULL);
    //offset_deducted +=sizeof(header->offset);
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

error write_zip_to_socket(array_of_strings_t* files, s_socket* socket) {
    local_file_header_t* local_files_headers = calloc(files->size, sizeof(local_file_header_t));
    if(local_files_headers == NULL){
        return FAIL;
    }
    central_directory_header_t* central_directory_headers = calloc(files->size, sizeof(central_directory_header_t));
    if(central_directory_headers == NULL){
        return FAIL;
    }
    extra_field_t* extra_fields = calloc(files->size, sizeof(extra_field_t));
    if(extra_fields == NULL){
        return FAIL;
    }

    uint64_t central_directory_offset = 0;
    uint64_t central_directory_size = 0;
    for (int i = 0; i < files->size; i++) {
        char *filename;
        error err = string_array_get(files, i, &filename);
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

        local_files_headers[i].signature = ZIP_LOCAL_FILE_HEADER_SIGNATURE;
        local_files_headers[i].version_needed = 20;
        local_files_headers[i].general_purpose_bit_flag = 0;
        local_files_headers[i].compression_method = 0;  // STORE
        local_files_headers[i].last_mod_file_time = 0;
        local_files_headers[i].last_mod_file_date = 0;
        local_files_headers[i].crc32 = crc;
        local_files_headers[i].compressed_size = 0;
        local_files_headers[i].uncompressed_size = 0;
        local_files_headers[i].file_name_length = strlen(filename);
        local_files_headers[i].extra_field_length = 0;

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
        central_directory_headers[i].disk_number_start = 0;0xFFFF;
        central_directory_headers[i].internal_file_attributes = 0;
        central_directory_headers[i].external_file_attributes = 0;
        central_directory_headers[i].relative_offset_of_local_header = central_directory_offset;

        extra_fields[i].signature = 0x0001;
        extra_fields[i].size_extra_field = 16;
        extra_fields[i].compressed_size = file_size;
        extra_fields[i].uncompressed_size = file_size;
        //extra_fields[i].offset = 0;

        central_directory_offset += LOCAL_FILE_HEADER_SIZE + local_files_headers[i].extra_field_length + strlen(filename) + file_size;// + 24;
        central_directory_size += CENTRAL_DIRECTORY_HEADER_SIZE + central_directory_headers[i].extra_field_length + strlen(filename);
    }

    for (int i = 0; i < files->size; i++) {
        write_local_file_header(socket, &local_files_headers[i]);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        socket_write(socket, filename, local_files_headers[i].file_name_length, NULL);
        offset_deducted += local_files_headers[i].file_name_length;
        //write_extra_field(socket, &extra_fields[i]);

        FILE* f = fopen(filename, "rb");
        if(f == NULL){
            return FAIL;
        }

        char buffer[READ_FILE_BUFFER];
        size_t bytes_read;
        while((bytes_read = fread(buffer, 1, READ_FILE_BUFFER, f)) > 0){
            offset_deducted += bytes_read;
            socket_write(socket, buffer, bytes_read, NULL);
        }

        fclose(f);
//        uint32_t signature = DATA_DESCRIPTOR_SIGNATURE;
//        socket_write(socket, &signature, sizeof(uint32_t), NULL);
//        offset_deducted +=sizeof(uint32_t);
//        socket_write(socket, &central_directory_headers[i].crc32, sizeof(uint32_t), NULL);
//        offset_deducted +=sizeof(uint32_t);
//        socket_write(socket, &extra_fields[i].compressed_size, sizeof(uint64_t), NULL);
//        offset_deducted +=sizeof(uint64_t);
//        socket_write(socket, &extra_fields[i].uncompressed_size, sizeof(uint64_t), NULL);
//        offset_deducted += sizeof(uint64_t);
    }

    for (int i = 0; i < files->size; i++) {
        write_central_directory_header(socket, &central_directory_headers[i]);

        char *filename;
        error err = string_array_get(files, i, &filename);
        if(err != SUCCESS){
            return err;
        }
        socket_write(socket, filename, central_directory_headers[i].file_name_length, NULL);
        offset_deducted += central_directory_headers[i].file_name_length;
        write_extra_field(socket, &extra_fields[i]);
    }

    //central_directory_offset += 24;
    end_of_central_directory_record_t eocd = {0};
    eocd.signature = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE;
    eocd.size_of_eocd_minus_12 = 44;
    eocd.version_made_by = 45;
    eocd.version_needed = 45;
    eocd.number_on_disk = 0;
    eocd.disk_where_central_directory_starts = 0;
    eocd.number_of_central_directories_on_disk = files->size;
    eocd.number_of_central_directories = files->size;
    eocd.size_of_cental_directory = central_directory_size;
    eocd.offset_of_start_of_central_directory = central_directory_offset;
    write_end_of_central_directory_record(socket, &eocd);

    uint32_t sig = ZIP_LOCATOR_SIGNATURE;
    uint32_t nr_disk = 0;
    uint32_t disks = 1;
    uint64_t end = central_directory_offset + central_directory_size;// PUT HERE THE FORMULA//;
    socket_write(socket, &sig, sizeof(uint32_t), NULL);
    offset_deducted +=sizeof(uint32_t);
    socket_write(socket, &nr_disk, sizeof(uint32_t), NULL);
    offset_deducted +=sizeof(uint32_t);
    socket_write(socket, &end, sizeof(uint64_t ), NULL);
    offset_deducted +=sizeof(uint64_t );
    socket_write(socket,&disks, sizeof(uint32_t ), NULL);
    offset_deducted +=sizeof(uint32_t );

    uint32_t directoryEndSignature = 101010256;
    uint16_t zeroValue = 0;
    uint16_t maxValue = 0xFFFF;
    uint32_t maxValue32 = 0xFFFFFFFF;

    socket_write(socket,&directoryEndSignature, sizeof(uint32_t ), NULL);
    offset_deducted +=sizeof(uint32_t );
    socket_write(socket,&zeroValue, sizeof(uint16_t ), NULL);
    offset_deducted +=sizeof(uint16_t );
    socket_write(socket,&zeroValue, sizeof(uint16_t ), NULL);
    offset_deducted +=sizeof(uint16_t );
    socket_write(socket,&maxValue, sizeof(uint16_t ), NULL);
    offset_deducted +=sizeof(uint16_t );
    socket_write(socket,&maxValue, sizeof(uint16_t ), NULL);
    offset_deducted +=sizeof(uint16_t );
    socket_write(socket,&maxValue32, sizeof(uint32_t), NULL);
    offset_deducted +=sizeof(uint32_t );
    socket_write(socket,&maxValue32, sizeof(uint32_t), NULL);
    offset_deducted +=sizeof(uint32_t );
    socket_write(socket,&zeroValue, sizeof(uint16_t ), NULL);
    offset_deducted +=sizeof(uint16_t );

    return SUCCESS;
}