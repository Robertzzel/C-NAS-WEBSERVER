//
// Created by robert on 6/23/24.
//

#include "http_request.h"

error parse_http_header(char *line, http_request_t *request);
error parse_http_request_line(char *line, http_request_t *request);
error http_request_init(http_request_t *request);

error http_request_t__from_bytes(char* message, http_request_t *request) {
    error err = http_request_init(request);
    if(err != SUCCESS) {
        return err;
    }

    list_strings_t parts;
    err = string_split(message, HTTP_MESSAGE_DELIMITER, &parts);
    if(err != SUCCESS){
        return err;
    }

    char *request_line = NULL;
    err = list_strings_t__get(&parts, 0, &request_line);
    if(err != SUCCESS){
        return err;
    }

    err = parse_http_request_line(request_line, request);
    if(err != SUCCESS) {
        list_strings_t__free(&parts);
        return err;
    }

    for(int i = 1; i < parts.size - 1; ++i) {
        char *header_line = NULL;
        err = list_strings_t__get(&parts, i, &header_line);
        if(err != SUCCESS){
            return err;
        }
        err = parse_http_header(header_line, request);
        if(err != SUCCESS){
            return err;
        }
    }

    char* last_part = NULL;
    err = list_strings_t__get(&parts, parts.size - 1, &last_part);
    if(err != SUCCESS){
        return FAIL;
    }
    request->body = strdup(last_part);
    if(request->body == NULL){
        return FAIL;
    }

    list_strings_t__free(&parts);
    return SUCCESS;
}

error parse_http_header(char *line, http_request_t *request) {
    char* delimiter = strchr(line, ':');
    if(delimiter == NULL){
        return FAIL;
    }
    long delimiter_index = delimiter-line;
    error err = list_strings_t__add(&request->header_names, line, delimiter_index);
    if(err != SUCCESS){
        return err;
    }

    if(strlen(delimiter) < 2) {
        return FAIL;
    }

    char* value;
    int value_end_index;
    err = trim_whitespace(delimiter + 1, &value, &value_end_index);
    if(err != SUCCESS){
        return err;
    }

    err = list_strings_t__add(&request->headers_values, value, value_end_index);
    if(err != SUCCESS){
        return err;
    }

    return SUCCESS;
}

error parse_http_request_line(char* line, http_request_t *request) {
    char* space_character = strchr(line, ' ');
    if(space_character == NULL){
        return FAIL;
    }
    unsigned long method_end_index = space_character - line;
    request->method = strndup(line, method_end_index);
    if(request->method == NULL){
        return FAIL;
    }
    line += method_end_index + 1;

    space_character = strchr(line, ' ');
    if(space_character == NULL){
        return FAIL;
    }
    unsigned long uri_end_index = space_character - line;
    request->uri = strndup(line, uri_end_index);
    if(request->uri == NULL){
        return FAIL;
    }

    size_t version_string_size = strlen(space_character + 1);
    if(version_string_size < 1){
        return FAIL;
    }
    request->version = strdup(space_character + 1);
    if(request->version == NULL){
        return FAIL;
    }

    return SUCCESS;
}

error http_request_t__free(http_request_t *request){
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
    if(request->body != NULL){
        free(request->body);
        request->body = NULL;
    }
    list_strings_t__free(&request->header_names);
    list_strings_t__free(&request->headers_values);

    return SUCCESS;
}

error http_request_init(http_request_t *request) {
    request->method = NULL;
    request->uri = NULL;
    request->version = NULL;
    error err = list_strings_t__new(&request->header_names);
    if(err != SUCCESS){
        return err;
    }
    err = list_strings_t__new(&request->headers_values);
    if(err != SUCCESS){
        return err;
    }
    request->body = NULL;

    return SUCCESS;
}