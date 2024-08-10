//
// Created by robert on 6/23/24.
//

#include "http_request.h"

LIST(list_char) http_request_t__parse_header(char* header_line);

request_t* request_from_bytes(buffered_socket_t* reader) {
    request_t* request = xmalloc(sizeof(request_t));

    request->method = buffered_socket_read_until_or_max(reader, ' ', 8);
    if (request->method == NULL){
        http_request_t__free(request);
        return NULL;
    }
    LIST_REMOVE_END(request->version, 1);
    LIST_ADD(request->method, (char)0);

    request->uri = buffered_socket_read_until_or_max(reader, ' ', 64);
    if(request->uri == NULL){
        http_request_t__free(request);
        return NULL;
    }
    LIST_REMOVE_END(request->version, 1);
    LIST_ADD(request->method, (char)0);

    request->version = buffered_socket_read_until_or_max(reader, '\n', 32);
    LIST_REMOVE_END(request->version, 2);
    LIST_ADD(request->method, (char)0);

    LIST(char)* header_delimiter = LIST_NEW(char, 0, 1);
    LIST_ADD(header_delimiter, '=');

    LIST(char)* header = buffered_socket_read_until_or_max(reader, '\n', 128);
    while (header->size > 2) { // more than \r\n
        LIST(list_char)* header_pars = string__split(header, header_delimiter);
        if(header_pars->size >= 2) {

        }

        for(size_t i = 0; i < header_pars->size; i++) {
            LIST(char)* l = LIST_GET(header_pars, i);
            LIST_FREE(l);
        }
        LIST_FREE(header_pars);
    }
    //
//    list_string_t *parts = string__split(message, HTTP_MESSAGE_DELIMITER);
//
//    char* request_line = list_strings__get(parts, 0);
//    list_string_t* request_line_parts = string__split(request_line, " ");
//    if(request_line_parts == NULL || request_line_parts->size != 3){
//        return NULL;
//    }
//
//    //request_t* request = xmalloc(sizeof(request_t));
//    char* method = list_strings__get(request_line_parts, 0);
//    request->method = string__from(method, strlen(method));
//
//    char* uri = list_strings__get(request_line_parts, 1);
//    char* found_question_mark = strchr(uri, '?');
//    if(found_question_mark == NULL ){
//        request->uri = string__from(uri, strlen(uri));
//    } else {
//        request->uri = string__substring(uri, 0, found_question_mark - uri);
//    }
//    //request->uri = string__copy(list_strings__get(request_line_parts, 1));
//
//    char* version = list_strings__get(request_line_parts, 2);
//    request->version = string__from(version, strlen(version));
//
//    list_strings__free(request_line_parts);
//
//    request->header_names = list_strings__new(parts->size - 2);
//    request->headers_values = list_strings__new(parts->size - 2);
//    int i;
//    for(i = 1; i < parts->size; ++i) {
//        char* header_line = list_strings__get(parts, i);
//
//        if(string__is_empty(header_line)) {
//            break;
//        }
//
//        list_string_t *key_value_pair = http_request_t__parse_header(header_line);
//        if(key_value_pair == NULL){
//            break;
//        }
//
//        char* key = list_strings__get(key_value_pair, 0);
//        char* value = list_strings__get(key_value_pair, 1);
//        list_strings__add(request->header_names, key);
//        list_strings__add(request->headers_values, value);
//
//        list_strings__free(key_value_pair);
//    }
//
//    char* last_part = list_strings__get(parts, i + 1);
//    request->body = string__from(last_part, strlen(last_part));
//
//    list_strings__free(parts);
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
    char* value = string__from(delimiter + 1, strlen(delimiter) + 1);

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

void http_request_t__free(request_t *request){
    if(request->method != NULL){
        LIST_FREE(request->method);
        request->method = NULL;
    }
    if(request->uri != NULL){
        LIST_FREE(request->uri);
        request->uri = NULL;
    }
    if(request->version != NULL){
        LIST_FREE(request->version);
        request->version = NULL;
    }
    if(request->body != NULL){
        LIST_FREE(request->body);
        request->body = NULL;
    }
    for (int i = 0; i < request->headers_values->size; ++i) {
        LIST_FREE(LIST_GET(request->headers_values, i));
    }
    LIST_FREE(request->headers_values);

    for (int i = 0; i < request->headers_names->size; ++i) {
        LIST_FREE(LIST_GET(request->headers_names, i));
    }
    LIST_FREE(request->headers_names);

    free(request);
}

char* http_request_t___get_form_value(request_t* request, char* key) {
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
            char* r = list_strings__get(key_value_pair, 1);
            result = string__from(r, strlen(r));
        }

        list_strings__free(key_value_pair);
    }
    return result;
}