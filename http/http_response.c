//
// Created by robert on 7/3/24.
//

#include "http_response.h"

http_response_t* http_response_t__new(){
    http_response_t* response = xmalloc(sizeof(http_response_t));
    response->body = NULL;
    response->version = xstrdup("HTTP/1.1");
    response->header_names = list_strings_t__new();
    response->header_values = list_strings_t__new();
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
    list_strings_t__free(response->header_names);
    list_strings_t__free(response->header_values);
    free(response);
}

void http_response_t__set_version(http_response_t* response, char* version){
    response->version = xstrdup(version);
}

bool http_response_t__set_status(http_response_t* response, int status){
    if(status < 100 || status > 599) {
        return false;
    }
    response->status = status;
    return true;
}

void http_response_t__set_body(http_response_t* response, char* body){
    response->body = xstrdup(body);
}

bool http_response_t__add_header(http_response_t* response, char* name, char* value) {
    int i;
    for(i=0;i<response->header_names->size;++i){
        char* header_name = list_strings_t__get(response->header_names, i);
        if(header_name == NULL){
            return false;
        }
        if(strcmp(name, header_name) != 0) {
            continue;
        }
        return list_strings_t__replace(response->header_values, i, value, strlen(value));
    }

    list_strings_t__add(response->header_names, name, strlen(name));
    list_strings_t__add(response->header_values, value, strlen(value));
    return true;
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
        char *name, *value;
        name = list_strings_t__get(response->header_names, i);
        if(name == NULL){
            return NULL;
        }
        value = list_strings_t__get(response->header_values, i);
        if(value == NULL){
            return NULL;
        }
        response_size += strlen(name) + 2 + strlen(value);
        response_size += 2; // delimiter
    }
    response_size += 2;
    //body
    if(response->body != NULL){
        response_size += strlen(response->body);
    }
    response_size += 2; // delimiter

    char* string = xmalloc(sizeof(char) * (response_size + 1) );
    *string = 0;

    strcat(string, response->version);
    strcat(string, " ");
    char status[4];
    sprintf(status, "%d", response->status);
    strcat(string, status);
    strcat(string, " ");
    strcat(string, "OK");
    strcat(string, "\r\n");

    for(i=0;i<response->header_names->size;++i){
        char *name, *value;
        name = list_strings_t__get(response->header_names, i);
        if(name == NULL){
            return NULL;
        }
        value = list_strings_t__get(response->header_values, i);
        if(value == NULL){
            return NULL;
        }
        strcat(string, name);
        strcat(string, ": ");
        strcat(string, value);
        strcat(string, "\r\n");
    }
    strcat(string, "\r\n");

    if(response->body != NULL){
        strcat(string, response->body);
    }
    strcat(string, "\r\n");

    return string;
}