#include "html_files.h"

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

error get_login_page(char** page) {
    return read_file("/home/robert/CLionProjects/untitled/templates/login.html", page);
}