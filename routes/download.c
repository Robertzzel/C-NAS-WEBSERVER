//
// Created by robert on 7/10/24.
//
#include "routes.h"

extern char* root_directory_path;

bool handle_download_route_post(request_t* request, reader_t* reader) {
    char* uri_file_name = request->uri + strlen("/download/");
    char* uri_file_name_end = strchr(request->uri, '?');
    char* file_name = xstrndup(uri_file_name, uri_file_name_end - uri_file_name);

    char* full_file_path = string__concatenate_strings(3, root_directory_path, "/", file_name);
    free(file_name);

    response_t response;
    response_new(&response);
    response_set_status(&response, 200);
    response_add_header(&response, "Connection", "close");
    response_add_header(&response, "Content-Type", "application/octet-stream");
    response_add_header(&response, "Content-Disposition", "attachment; filename=\"hello\"");
    char* string = response_to_bytes(&response);
    if(string == NULL) {
        response_free(&response);
        return false;
    }

    reader_write_buffer( reader, string, strlen(string)-2);

    char* files[] = {full_file_path, 0};

    bool success = write_zip_to_socket(files, reader);
    if(!success){
        response_free(&response);
        return false;
    }

    free(string);
    response_free(&response);

    return true;
}