//
// Created by robert on 7/3/24.
//

#include "http_response.h"

error http_response_new(http_response_t* response){
    response->body = NULL;
    response->version = strdup("HTTP/1.1");
    if(response->version == NULL) {
        return FAIL;
    }
    error err = string_array_new(&response->header_names);
    if(err != SUCCESS){
        return err;
    }
    err = string_array_new(&response->header_values);
    if(err != SUCCESS){
        return err;
    }
    response->status = 0;
    return SUCCESS;
}

error http_response_free(http_response_t* response){
    if(response->body != NULL) {
        free(response->body);
    }
    if(response->version != NULL) {
        free(response->version);
    }
    string_array_free(&response->header_names);
    string_array_free(&response->header_values);
    return SUCCESS;
}

error http_response_set_version(http_response_t* response, char* version){
    response->version = strdup(version);
    if(response->version == NULL){
        return FAIL;
    }
    return SUCCESS;
}

error http_response_set_status(http_response_t* response, int status){
    if(status < 100 || status > 599) {
        return FAIL;
    }
    response->status = status;
    return SUCCESS;
}

error http_response_set_body(http_response_t* response, char* body){
    response->body = strdup(body);
    if(response->body == NULL){
        return FAIL;
    }

    return SUCCESS;
}

error http_response_add_header(http_response_t* response, char* name, char* value) {
    int i;
    for(i=0;i<response->header_names.size;++i){
        char* header_name = NULL;
        error err = string_array_get(&(response->header_names), i, &header_name);
        if(err != SUCCESS){
            return FAIL;
        }
        if(strcmp(name, header_name) == 0) {
            err = string_array_replace(&response->header_values, i, value, strlen(value));
            if(err != SUCCESS) {
                return err;
            }
            return SUCCESS;
        }
    }

    error err = string_array_add(&response->header_names, name, strlen(name));
    if(err != SUCCESS){
        return err;
    }
    err = string_array_add(&response->header_values, value, strlen(value));
    if(err != SUCCESS){
        return err;
    }
    return SUCCESS;
}

error http_response_to_bytes(http_response_t* response, char** string) {
    unsigned long response_size = 0;
    // response line
    response_size += strlen(response->version) + 1;
    response_size += 3 + 1; // status code
    response_size += 2; // only OK message supported for now
    response_size += 2; // delimiter
    // headers
    int i = 0;
    for(i=0;i<response->header_names.size;++i){
        char *name, *value;
        error err = string_array_get(&response->header_names, i, &name);
        if(err != SUCCESS){
            return err;
        }
        err = string_array_get(&response->header_values, i, &value);
        if(err != SUCCESS){
            return err;
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

    *string = malloc(sizeof(char) * (response_size + 1) );
    if(*string == NULL) {
        return FAIL;
    }
    **string = 0;

    strcat(*string, response->version);
    strcat(*string, " ");
    char status[4];
    sprintf(status, "%d", response->status);
    strcat(*string, status);
    strcat(*string, " ");
    strcat(*string, "OK");
    strcat(*string, "\r\n");

    for(i=0;i<response->header_names.size;++i){
        char *name, *value;
        error err = string_array_get(&response->header_names, i, &name);
        if(err != SUCCESS){
            return err;
        }
        err = string_array_get(&response->header_values, i, &value);
        if(err != SUCCESS){
            return err;
        }
        strcat(*string, name);
        strcat(*string, ": ");
        strcat(*string, value);
        strcat(*string, "\r\n");
    }
    strcat(*string, "\r\n");

    if(response->body != NULL){
        strcat(*string, response->body);
    }
    strcat(*string, "\r\n");

    return SUCCESS;
}

error http_response_write_to_socket(http_response_t* response, s_socket* socket, int write_body) {
    socket_write(socket, response->version, strlen(response->version), NULL);
    socket_write(socket, " ", 1, NULL);
    char status[4];
    sprintf(status, "%d", response->status);
    socket_write(socket, status, 3, NULL);
    socket_write(socket, " OK\r\n", 5, NULL);

    for(int i=0;i<response->header_names.size;++i){
        char *name, *value;
        error err = string_array_get(&response->header_names, i, &name);
        if(err != SUCCESS){
            return err;
        }
        err = string_array_get(&response->header_values, i, &value);
        if(err != SUCCESS){
            return err;
        }
        socket_write(socket, name, strlen(name), NULL);
        socket_write(socket, ": ", 2, NULL);
        socket_write(socket, value, strlen(value), NULL);
        socket_write(socket, "\r\n", 2, NULL);
    }
    socket_write(socket, "\r\n", 2, NULL);

    if(write_body == 0){
        return SUCCESS;
    }

    if(response->body != NULL){
        socket_write(socket, response->body, strlen(response->body), NULL);
    }

    socket_write(socket, "\r\n", 2, NULL);

    return SUCCESS;
}