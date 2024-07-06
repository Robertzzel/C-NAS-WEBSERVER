//
// Created by robert on 7/5/24.
//

#include <string.h>
#include "file_utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE 0x02014b50
#define ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE 0x06054b50

#pragma pack(push, 1)
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
} CentralDirectoryHeader;

typedef struct {
    uint32_t signature;
    uint16_t number_of_this_disk;
    uint16_t number_of_the_disk_with_the_start_of_the_central_directory;
    uint16_t total_number_of_entries_in_the_central_directory_on_this_disk;
    uint16_t total_number_of_entries_in_the_central_directory;
    uint32_t size_of_the_central_directory;
    uint32_t offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number;
    uint16_t zip_file_comment_length;
} EndOfCentralDirectoryRecord;
#pragma pack(pop)

unsigned int m_crc32(const void *buf, size_t size) {
    // Simple CRC32 function placeholder; you might want to use a proper CRC32 implementation
    unsigned int crc = 0xFFFFFFFF;
    const unsigned char *p = buf;
    while (size--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

void write_zip_file(const char *zip_filename, const char **files, int file_count) {
    FILE *zip_file = fopen(zip_filename, "wb");
    if (!zip_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    unsigned int central_directory_offset = 0;
    unsigned int central_directory_size = 0;
    for (int i = 0; i < file_count; i++) {
        const char *filename = files[i];
        FILE *input_file = fopen(filename, "rb");
        if (!input_file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fseek(input_file, 0, SEEK_END);
        unsigned int file_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);
        void *file_data = malloc(file_size);
        if (!file_data) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        fread(file_data, 1, file_size, input_file);
        fclose(input_file);

        unsigned int crc = m_crc32(file_data, file_size);
        local_file_header_t local_header = {0};
        local_header.signature = ZIP_LOCAL_FILE_HEADER_SIGNATURE;
        local_header.version_needed = 20;
        local_header.general_purpose_bit_flag = 0;
        local_header.compression_method = 0;  // STORE
        local_header.last_mod_file_time = 0;
        local_header.last_mod_file_date = 0;
        local_header.crc32 = crc;
        local_header.compressed_size = file_size;
        local_header.uncompressed_size = file_size;
        local_header.file_name_length = strlen(filename);
        local_header.extra_field_length = 0;

        fwrite(&local_header, sizeof(local_header), 1, zip_file);
        fwrite(filename, 1, local_header.file_name_length, zip_file);
        fwrite(file_data, 1, file_size, zip_file);
    }

    for (int i = 0; i < file_count; i++) {
        const char *filename = files[i];
        FILE *input_file = fopen(filename, "rb");
        if (!input_file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fseek(input_file, 0, SEEK_END);
        unsigned int file_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);
        void *file_data = malloc(file_size);
        if (!file_data) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        fread(file_data, 1, file_size, input_file);
        fclose(input_file);

        unsigned int crc = m_crc32(file_data, file_size);

        CentralDirectoryHeader central_header = {0};
        central_header.signature = ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE;
        central_header.version_made_by = 20;
        central_header.version_needed = 20;
        central_header.compression_method = 0;  // STORE
        central_header.last_mod_file_time = 0;
        central_header.last_mod_file_date = 0;
        central_header.crc32 = crc;
        central_header.compressed_size = file_size;
        central_header.uncompressed_size = file_size;
        central_header.file_name_length = strlen(filename);
        central_header.relative_offset_of_local_header = central_directory_offset;

        fwrite(&central_header, sizeof(central_header), 1, zip_file);
        fwrite(filename, 1, central_header.file_name_length, zip_file);

        central_directory_offset += sizeof(local_file_header_t) + strlen(filename) + file_size;
        central_directory_size += sizeof(CentralDirectoryHeader) + strlen(filename);

        free(file_data);
    }

    EndOfCentralDirectoryRecord eocd = {0};
    eocd.signature = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE;
    eocd.total_number_of_entries_in_the_central_directory_on_this_disk = file_count;
    eocd.total_number_of_entries_in_the_central_directory = file_count;
    eocd.size_of_the_central_directory = central_directory_size;
    eocd.offset_of_start_of_central_directory_with_respect_to_the_starting_disk_number = central_directory_offset;

    fwrite(&eocd, sizeof(eocd), 1, zip_file);

    fclose(zip_file);
}