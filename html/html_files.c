//
// Created by robert on 7/3/24.
//

#include "html_files.h"



error write_home_page_to_socket(socket_t* socket) {
    TMPL_varlist *mylist;
    mylist = TMPL_add_var(0, "y", "sdsd", "x", "aa", 0 );

    FILE *f = tmpfile();
    if(f == NULL){
        return FAIL;
    }

    TMPL_write("/home/robert/CLionProjects/untitled/templates/home.html", 0, 0, mylist, f, stderr);
    rewind(f);

    char buffer[64];
    size_t bytes_read = fread(&buffer[0], 1, 64, f);
    while(bytes_read != 0) {
        socket_t__write(socket, buffer, bytes_read, NULL);
        bytes_read = fread(buffer, 1, 64, f);
    }

    fclose(f);
    return SUCCESS;
}

error get_home_page(char** page) {
    TMPL_varlist *mylist;
    mylist = TMPL_add_var(0, "y", "sdsd", "x", "aa", 0 );

    FILE *f = tmpfile();
    if(f == NULL){
        return FAIL;
    }

    TMPL_write("/home/robert/CLionProjects/untitled/templates/home.html", 0, 0, mylist, f, stderr);
    TMPL_free_varlist(mylist);

    long size = ftell(f);
    *page = malloc(sizeof(char) * (size + 1));
    if(*page == NULL){
        return FAIL;
    }
    rewind(f);

    fread(*page, 1, size, f);
    (*page)[size] = 0;

    fclose(f);
    return SUCCESS;
}