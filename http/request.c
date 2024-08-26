//
// Created by robert on 6/23/24.
//

#include "request.h"


char* get_method(reader_t* reader) {
    bytes_t* method_bytes = reader_read_until(reader, ' ', 8);
    if (method_bytes == NULL) {
        return NULL;
    }

    method_bytes->data[method_bytes->size-1] = 0;
    char* str = method_bytes->data;
    free(method_bytes);

    return str;
}

char* get_uri(reader_t* reader) {
    bytes_t* method_bytes = reader_read_until(reader, ' ', 64);
    if (method_bytes == NULL) {
        return NULL;
    }

    method_bytes->data[method_bytes->size-1] = 0;
    char* str = method_bytes->data;

    free(method_bytes);

    return str;
}

char* get_version(reader_t* reader) {
    bytes_t* method_bytes = reader_read_until(reader, '\n', 16);
    if (method_bytes == NULL) {
        return NULL;
    }

    method_bytes->data[method_bytes->size-2] = 0;
    char* str = method_bytes->data;

    free(method_bytes);

    return str;
}

char*** get_headers(reader_t* reader) {
    char* headers[40][2];

    size_t header_index = 0;
    while(1) {
        bytes_t* header_bytes = reader_read_until(reader, '\n', 128);
        if(header_bytes == NULL) {
            break;
        }
        char* header_content = bytes_to_string(header_bytes);
        free(header_bytes->data);
        free(header_bytes);

        if(strcmp(header_content, "\r\n") == 0) {
            free(header_content);
            break;
        }

        char* separator = strchr(header_content, ':');
        if(separator == NULL) { free(header_content); continue; }

        char* name = strndup(header_content, separator - header_content);
        char* value = strndup(separator + 2, strlen(separator + 2) - 2);

        free(header_content);

        headers[header_index][0] = name;
        headers[header_index][1] = value;

        header_index++;
    }

    char*** returned_headers = xcalloc((header_index + 2) * sizeof(char**));
    for (size_t i = 0; i < header_index ; ++i) {
        returned_headers[i] = xcalloc(3 * sizeof(char*));
        returned_headers[i][0] = headers[i][0];
        returned_headers[i][1] = headers[i][1];
    }

    return returned_headers;
}

request_t* request_from_bytes(reader_t* reader) {
    char* method = get_method(reader);
    if (method == NULL){
        return NULL;
    }

    char* uri = get_uri(reader);
    if(uri == NULL){
        free(method);
        return NULL;
    }

    char* version = get_version(reader);
    if (version == NULL) {
        free(method);
        free(uri);
        return NULL;
    }

    char*** headers = get_headers(reader);

    request_t* request = xmalloc(sizeof(request_t));
    request->method = method;
    request->uri = uri;
    request->version = version;
    request->headers = headers;

    return request;
}


void request_free(request_t *request){
    if(request->method != NULL){
        free(request->method);
        request->method = NULL;
    }
    if(request->uri != NULL){
        free(request->uri);
        request->uri = NULL;
    }
    if(request->version != NULL){
        free(request->version);
        request->version = NULL;
    }
    for (size_t i = 0; request->headers[i] != 0 ; i++) {
        free(request->headers[i][0]);
        free(request->headers[i][1]);
        free(request->headers[i]);
    }
    free(request->headers);

    free(request);
}

char* request_header_value(request_t* request, char* name) {
    for (size_t i = 0; request->headers[i] != 0 ; i++) {
        if(strcmp(request->headers[i][0], name) == 0) {
            return xstrdup(request->headers[i][1]);
        }
    }
    return NULL;
}

char* request_form_value(char* body, char* label) {
    char* label_start = strstr(body, label);
    if(label_start == NULL) {
        return NULL;
    }
    char* value_start = label_start + strlen(label) + 1; // skip "="

    char* value_end = strchr(value_start, '&');
    if(value_end == NULL) {
        return xstrdup(value_start);
    }

    return xstrndup(value_start, value_end - value_start);
}