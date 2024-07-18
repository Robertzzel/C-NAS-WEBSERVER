//
// Created by robert on 7/3/24.
//

#include "http_response.h"

http_response_t* http_response_t__new(){
    http_response_t* response = xmalloc(sizeof(http_response_t));
    response->body = NULL;
    response->version = string__copy("HTTP/1.1");
    response->header_names = list_strings__new(3);
    response->header_values = list_strings__new(3);
    response->status = 0;
    return response;
}

void http_response_t__free(http_response_t* response){
    if(response->body != NULL) {
        free(response->body);
    }
    if(response->version != NULL) {
        free(response->version);
    }
    list_strings__free(response->header_names);
    list_strings__free(response->header_values);
    free(response);
}

void http_response_t__set_version(http_response_t* response, char* version){
    if(version != NULL) {
        free(response->version);
    }
    response->version = string__copy(version);
}

bool http_response_t__set_status(http_response_t* response, int status){
    if(status < 100 || status > 599) {
        return false;
    }
    response->status = status;
    return true;
}

void http_response_t__set_body(http_response_t* response, char* body){
    if(response->body != NULL) {
        free(response->body);
    }
    response->body = string__copy(body);
}

void http_response_t__add_header(http_response_t* response, char* name, char* value) {
    for(int i=0; i<response->header_names->size; ++i){
        char* header_name = list_strings__get(response->header_names, i);
        if(strcmp(name, header_name) == 0) {
            list_strings__replace(response->header_values, i, value);
            return;
        }
    }

    list_strings__add(response->header_names, name);
    list_strings__add(response->header_values, value);
}

char* http_response_t__to_bytes(http_response_t* response) {
    unsigned long response_size = 0;
    // response line
    response_size += strlen(response->version) + 1;
    response_size += 3 + 1; // status code
    response_size += 2; // only OK message supported for now
    response_size += 2; // delimiter
    // headers
    int i = 0;
    for(i=0;i<response->header_names->size;++i){
        char* name = list_strings__get(response->header_names, i);
        char* value = list_strings__get(response->header_values, i);
        response_size += strlen(name) + 2 + strlen(value);
        response_size += 2; // delimiter
    }
    response_size += 2;
    //body
    if(response->body != NULL){
        response_size += strlen(response->body);
    }
    response_size += 2; // delimiter

    char* result = string__new(response_size);
    strcat(result, response->version);
    strcat(result, " ");
    char status[4];
    sprintf(status, "%d", response->status);
    strcat(result, status);
    strcat(result, " ");
    strcat(result, "OK");
    strcat(result, "\r\n");

    for(int i=0;i<response->header_names->size;++i){
        char* name = list_strings__get(response->header_names, i);
        char* value = list_strings__get(response->header_values, i);
        strcat(result, name);
        strcat(result, ": ");
        strcat(result, value);
        strcat(result, "\r\n");
    }
    strcat(result, "\r\n");

    if(response->body != NULL){
        strcat(result, response->body);
    }
    strcat(result, "\r\n");

    return result;
}