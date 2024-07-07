//
// Created by robert on 7/5/24.
//

#include "routes.h"
#include "zlib.h"
#define DOWNLOAD_BUFFER_SIZE (8 * 1024)

error handle_root_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = get_home_page(&response.body);
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string), NULL);
    http_response_free(&response);
    free(string);

    return SUCCESS;
}

error handle_not_found_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 404);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }

    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string), NULL);
    free(string);
    http_response_free(&response);
    return SUCCESS;
}

error handle_download_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Type", "application/octet-stream");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Disposition", "attachment; filename=\"hello\"");
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string) - 2, NULL);

    array_of_strings_t files;
    string_array_new(&files);
    //string_array_add(&files, "/home/robert/start.sh", strlen("/home/robert/start.sh"));
    //string_array_add(&files, "/home/robert/Workspace/iommu.sh", strlen("/home/robert/Workspace/iommu.sh"));
    //string_array_add(&files, "/home/robert/Downloads/virtio-win-0.1.240.iso", strlen("/home/robert/Downloads/virtio-win-0.1.240.iso"));
    string_array_add(&files, "/home/robert/Downloads/Win11_23H2_English_x64v2.iso", strlen("/home/robert/Downloads/Win11_23H2_English_x64v2.iso"));
    err = write_zip_to_socket(&files, conn);
    if(err != SUCCESS){
        return err;
    }
    string_array_free(&files);

    socket_write(conn, "\r\n", 2, NULL);
    free(string);
    http_response_free(&response);

    return SUCCESS;
}