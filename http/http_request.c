//
// Created by robert on 6/23/24.
//

#include "http_request.h"

list_string_t* http_request_t__get_method_uri_version(char* request_line);
list_string_t* http_request_t__parse_header(char* header_line);

http_request_t* http_request_t__from_bytes(char* message) {
    list_string_t *parts = string__split(message, HTTP_MESSAGE_DELIMITER);

    char* request_line = list_strings__get(parts, 0);
    list_string_t* request_line_parts = string__split(request_line, " ");
    if(request_line_parts == NULL || request_line_parts->size != 3){
        return NULL;
    }

    http_request_t* request = xmalloc(sizeof(http_request_t));
    request->method = string__copy(list_strings__get(request_line_parts, 0));
    request->uri = string__copy(list_strings__get(request_line_parts, 1));
    request->version = string__copy(list_strings__get(request_line_parts, 2));
    list_strings__free(request_line_parts);

    request->header_names = list_strings__new(parts->size - 2);
    request->headers_values = list_strings__new(parts->size - 2);
    int i;
    for(i = 1; i < parts->size; ++i) {
        char* header_line = list_strings__get(parts, i);

        if(string__is_empty(header_line)) {
            break;
        }

        list_string_t *key_value_pair = http_request_t__parse_header(header_line);
        if(key_value_pair == NULL){
            break;
        }

        char* key = list_strings__get(key_value_pair, 0);
        char* value = list_strings__get(key_value_pair, 1);
        list_strings__add(request->header_names, key);
        list_strings__add(request->headers_values, value);

        list_strings__free(key_value_pair);
    }

    char* last_part = list_strings__get(parts, i + 1);
    request->body = string__copy(last_part);

    list_strings__free(parts);
    return request;
}

list_string_t* http_request_t__parse_header(char* header_line) {
    char* delimiter = strchr(header_line, ':');
    if(delimiter == NULL){
        return NULL;
    }

    unsigned long header_key_size = delimiter - header_line;
    unsigned long header_value_size = strlen(delimiter + 1);
    if(header_key_size < 1 || header_value_size < 1){
        return NULL;
    }

    char* key = string__substring(header_line, 0, header_key_size);
    char* value = string__copy(delimiter + 1);

    char* clean_key = string__trim_whitespace(key);
    char* clean_value = string__trim_whitespace(value);
    free(key);
    free(value);

    list_string_t* result = list_strings__new(2);
    list_strings__add(result, clean_key);
    list_strings__add(result, clean_value);
    free(clean_value);
    free(clean_key);
    return result;
}

void http_request_t__free(http_request_t *request){
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
    list_strings__free(request->header_names);
    list_strings__free(request->headers_values);
    free(request);
}

char* http_request_t___get_form_value(http_request_t* request, char* key) {
    list_string_t* form_parts = string__split(request->body, "&");
    if(form_parts->size < 1) {
        free(form_parts);
        return NULL;
    }

    char* result = NULL;
    for(int i = 0; i < form_parts->size; ++i){
        char* part = list_strings__get(form_parts, i);
        list_string_t* key_value_pair = string__split(part, "=");
        if(key_value_pair->size < 2){
            list_strings__free(key_value_pair);
            continue;
        }

        char* form_part_key = list_strings__get(key_value_pair, 0);
        if(strcmp(form_part_key, key) == 0) {
            result = string__copy(list_strings__get(key_value_pair, 1));
        }

        list_strings__free(key_value_pair);
    }
    return result;
}