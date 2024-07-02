//
// Created by robert on 6/23/24.
//

#include "http_request.h"
#include "m_string.h"

error parse_http_header(char *line, HttpRequest *request);
error parse_http_request_line(char *line, HttpRequest *request);

error free_http_request(HttpRequest *request){
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
    string_array_free(request->header_names, request->header_count);
    request->header_names = NULL;
    string_array_free(request->headers_values, request->header_count);
    request->headers_values = NULL;
}

error parse_http_request(char* message, HttpRequest *request) {
    request->method = NULL;
    request->uri = NULL;
    request->version = NULL;
    request->header_names = NULL;
    request->headers_values = NULL;
    request->body = NULL;
    request->header_count = 0;
    char line_delimiter[] = "\r\n";

    char** parts;
    int number_of_parts;
    error err = m_string_split(message, line_delimiter, &parts, &number_of_parts);
    if(err != SUCCESS){
        return err;
    }

    err = parse_http_request_line(*parts, request);
    if(err != SUCCESS) {
        string_array_free(parts, number_of_parts);
        return err;
    }

    int number_of_headers = number_of_parts - 2;
    if(number_of_headers > 0) {
        request->header_names = malloc(sizeof(char*) * number_of_headers);
        if(request->header_names == NULL){
            string_array_free(parts, number_of_parts);
            return FAIL;
        }
        request->headers_values = malloc(sizeof(char*) * number_of_headers);
        if(request->headers_values == NULL){
            string_array_free(parts, number_of_parts);
            free(request->header_names);
            request->header_names = NULL;
            return FAIL;
        }

        int i;
        for(i = 1; i < number_of_parts - 1; ++i) {
            parse_http_header(*(parts+i), request);
        }
    }

    char* last_part = *(parts+number_of_parts-1);
    request->body = strdup(last_part);
    if(request->body == NULL){
        return FAIL;
    }

    string_array_free(parts, number_of_parts);
    return SUCCESS;
}

error parse_http_header(char *line, HttpRequest *request) {
    char* delimiter = strchr(line, ':');
    if(delimiter == NULL){
        return FAIL;
    }
    long delimiter_index = delimiter-line;
    char* key = strndup(line, delimiter_index);
    if(key == NULL){
        return FAIL;
    }
    request->header_names[request->header_count] = key;

    if(strlen(delimiter) < 2) {
        return FAIL;
    }

    char* value;
    int value_end_index;
    error err = trim_whitespace(delimiter + 1, &value, &value_end_index);
    if(err != SUCCESS){
        return err;
    }

    value = strndup(value, value_end_index);
    if(value == NULL){
        return FAIL;
    }
    request->headers_values[request->header_count] = value;

    request->header_count++;
    return SUCCESS;
}

error parse_http_request_line(char* line, HttpRequest *request) {
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