//
// Created by robert on 7/9/24.
//

#include "zip.h"
#include "file_utils.h"


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

bool write_local_file_header(reader_t *socket, const char* filename, uint32_t crc32) {
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

    reader_write_buffer(socket, local_file_header, LOCAL_FILE_HEADER_SIZE);
    reader_write_buffer(socket, (char*)filename, strlen(filename));
    FILE* f = fopen(filename, "rb");
    if(f == NULL){
        return false;
    }

    char buffer[READ_FILE_BUFFER_SIZE];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, READ_FILE_BUFFER_SIZE, f)) > 0){
        reader_write_buffer(socket, buffer, bytes_read);
    }

    fclose(f);
    return true;
}

void write_central_directory_header(reader_t* socket, central_directory_header_t * header, char* filename) {
    reader_write_buffer(socket, &header->signature, sizeof(header->signature));
    reader_write_buffer(socket, &header->version_made_by, sizeof(header->version_made_by));
    reader_write_buffer(socket, &header->version_needed, sizeof(header->version_needed));
    reader_write_buffer(socket, &header->general_purpose_bit_flag, sizeof(header->general_purpose_bit_flag));
    reader_write_buffer(socket, &header->compression_method, sizeof(header->compression_method));
    reader_write_buffer(socket, &header->last_mod_file_time, sizeof(header->last_mod_file_time));
    reader_write_buffer(socket, &header->last_mod_file_date, sizeof(header->last_mod_file_date));
    reader_write_buffer(socket, &header->crc32, sizeof(header->crc32));
    reader_write_buffer(socket, &header->compressed_size, sizeof(header->compressed_size));
    reader_write_buffer(socket, &header->uncompressed_size, sizeof(header->uncompressed_size));
    reader_write_buffer(socket, &header->file_name_length, sizeof(header->file_name_length));
    reader_write_buffer(socket, &header->extra_field_length, sizeof(header->extra_field_length));
    reader_write_buffer(socket, &header->file_comment_length, sizeof(header->file_comment_length));
    reader_write_buffer(socket, &header->disk_number_start, sizeof(header->disk_number_start));
    reader_write_buffer(socket, &header->internal_file_attributes, sizeof(header->internal_file_attributes));
    reader_write_buffer(socket, &header->external_file_attributes, sizeof(header->external_file_attributes));
    reader_write_buffer(socket, &header->relative_offset_of_local_header, sizeof(header->relative_offset_of_local_header));
    reader_write_buffer(socket, filename, header->file_name_length);
    reader_write_buffer(socket, &header->extra_field.signature, sizeof(header->extra_field.signature));
    reader_write_buffer(socket, &header->extra_field.size_extra_field, sizeof(header->extra_field.size_extra_field));
    reader_write_buffer(socket, &header->extra_field.uncompressed_size, sizeof(header->extra_field.uncompressed_size));
    reader_write_buffer(socket, &header->extra_field.compressed_size, sizeof(header->extra_field.compressed_size));
}

uint32_t m_crc32(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return 0xe320bbde;
    }
    uint32_t crc = 0xe320bbde;
    rewind(file);

    char buffer[READ_FILE_BUFFER_SIZE];
    size_t bytes_read;
    while((bytes_read = fread(buffer, READ_FILE_BUFFER_SIZE, 1, file)) > 0) {
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
    crc = crc ^ 0xe320bbde;
    return crc;
}

bool write_zip_to_socket(char** file_paths, reader_t* socket) {
    int nr_files;
    for (nr_files = 0; file_paths[nr_files] != 0 ; ++nr_files) {}

    central_directory_header_t *central_directory_headers = xcalloc(sizeof(central_directory_header_t) * nr_files);

    uint64_t central_directory_offset = 0;
    uint64_t central_directory_size = 0;
    for (int i = 0; i < nr_files; i++) {
        char *filename = file_paths[i];
        uint64_t file_size = get_file_size(filename);
        if (file_size == -1) {
            continue;
        }

        uint32_t crc = m_crc32(filename);

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
        central_directory_size +=
                CENTRAL_DIRECTORY_HEADER_SIZE + central_directory_headers[i].extra_field_length + strlen(filename);
    }

    for (int i = 0; i < nr_files; i++) {
        char *filename = file_paths[i];
        write_central_directory_header(socket, &central_directory_headers[i], filename);
    }
    free(central_directory_headers);

    char buffer[ZIP_END_OF_CENTRAL_DIRECTORY];
    *(uint32_t *) (buffer) = ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE; // signature
    *(uint64_t *) (buffer + 4) = 44; // size_of_eocd_minus_12
    *(uint16_t *) (buffer + 12) = 45; // version_made_by
    *(uint16_t *) (buffer + 14) = 45; // version_needed
    *(uint32_t *) (buffer + 16) = 0; // number_on_disk
    *(uint32_t *) (buffer + 20) = 0; // disk_where_central_directory_starts
    *(uint64_t *) (buffer + 24) = nr_files; // number_of_central_directories_on_disk
    *(uint64_t *) (buffer + 32) = nr_files; // number_of_central_directories
    *(uint64_t *) (buffer + 40) = central_directory_size; // size_of_cental_directory
    *(uint64_t *) (buffer + 48) = central_directory_offset; // offset_of_start_of_central_directory
    reader_write_buffer(socket, buffer, ZIP_END_OF_CENTRAL_DIRECTORY);

    char zip_locator_buffer[ZIP_LOCATOR_SIZE];
    *(uint32_t *) (zip_locator_buffer) = ZIP_LOCATOR_SIGNATURE; // signature
    *(uint32_t *) (zip_locator_buffer +
                   4) = 0; // number of the disk with the read of the zip64 write of central directory
    *(uint64_t *) (zip_locator_buffer + 8) = central_directory_offset +
                                             central_directory_size; // relative offset of the zip64 write of central directory record
    *(uint32_t *) (zip_locator_buffer + 16) = 1; // total number of disks
    reader_write_buffer(socket, zip_locator_buffer, ZIP_LOCATOR_SIZE);

    char end_of_record_buffer[ZIP_DIRECTORY_END_SIZE];
    *(uint32_t *) (end_of_record_buffer) = 101010256; // directoryEndSignature
    *(uint16_t *) (end_of_record_buffer + 4) = 0;
    *(uint16_t *) (end_of_record_buffer + 6) = 0;
    *(uint16_t *) (end_of_record_buffer + 8) = 0xFFFF; // number of entries this disk
    *(uint16_t *) (end_of_record_buffer + 10) = 0xFFFF; // number of entries total
    *(uint32_t *) (end_of_record_buffer + 12) = 0xFFFFFFFF; // size of directory
    *(uint32_t *) (end_of_record_buffer + 16) = 0xFFFFFFFF; // read of directory
    *(uint16_t *) (end_of_record_buffer + 20) = 0; // byte size of eocd command
    reader_write_buffer(socket, end_of_record_buffer, ZIP_DIRECTORY_END_SIZE);

    return true;
}