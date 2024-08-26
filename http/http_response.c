//
// Created by robert on 7/3/24.
//

#include "http_response.h"

void response_new(response_t* response){
    response->body = NULL;
    response->version = xstrdup("HTTP/1.1");
    response->headers = xcalloc(1);
    response->status = 0;
}

void response_free(response_t* response){
    if(response->body != NULL) {
        free(response->body);
    }
    if(response->version != NULL) {
        free(response->version);
    }
    for (size_t i = 0; response->headers[i] != NULL ; ++i) {
        for (size_t j = 0; response->headers[i][j] != NULL ; ++j) {
            free(response->headers[i][j]);
        }
        free(response->headers[i]);
    }
    free(response->headers);
}

void response_set_version(response_t* response, char* version){
    if(version != NULL) {
        free(response->version);
    }
    response->version = xstrdup(version);
}

bool response_set_status(response_t* response, int status){
    if(status < 100 || status > 599) {
        return false;
    }
    response->status = status;
    return true;
}

void response_set_body(response_t* response, char* body){
    if(response->body != NULL) {
        free(response->body);
    }
    response->body = xstrdup(body);
}

void response_add_header(response_t* response, char* name, char* value) {
    int i;
    for(i=0; response->headers[i] != NULL; ++i){
        if(strcmp(name, response->headers[i][0]) == 0) {
            free(response->headers[i][1]);
            response->headers[i][1] = xstrdup(value);
            return;
        }
    }

    response->headers = xrealloc(response->headers, (i + 2) * sizeof(char**));
    response->headers[i] = xcalloc(3 * sizeof(char*));
    response->headers[i][0] = xstrdup(name);
    response->headers[i][1] = xstrdup(value);
    response->headers[i+1] = 0;
}

char* response_to_bytes(response_t* response) {
    unsigned long response_size = 0;
    // response line
    response_size += strlen(response->version) + 1;
    response_size += 3 + 1; // status code
    response_size += 2; // only OK message supported for now
    response_size += 2; // delimiter
    // headers
    for(int i=0;response->headers[i] != NULL;++i){
        char* name = response->headers[i][0];
        char* value = response->headers[i][1];
        response_size += strlen(name) + 2 + strlen(value);
        response_size += 2; // delimiter
    }
    response_size += 2;
    //body
    if(response->body != NULL){
        response_size += strlen(response->body);
    }
    response_size += 2; // delimiter

    char* result = xcalloc(response_size + 1);
    strcat(result, response->version);
    strcat(result, " ");
    char status[4];
    sprintf(status, "%d", response->status);
    strcat(result, status);
    strcat(result, " ");
    strcat(result, "OK");
    strcat(result, "\r\n");

    for(int i=0;response->headers[i] != NULL;++i){
        char* name = response->headers[i][0];
        char* value = response->headers[i][1];
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