#include "html_files.h"

char* get_home_page() {
    TMPL_varlist *mylist;
    mylist = TMPL_add_var(0, "y", "sdsd", "x", "aa", 0 );

    FILE *f = tmpfile();
    if(f == NULL){
        return NULL;
    }

    TMPL_write("/home/robert/CLionProjects/untitled/templates/home.html", 0, 0, mylist, f, stderr);
    TMPL_free_varlist(mylist);

    long size = ftell(f);
    char* page = xmalloc(sizeof(char) * (size + 1));
    rewind(f);

    fread(page, 1, size, f);
    page[size] = 0;

    fclose(f);
    return page;
}

char* get_login_page() {
    return read_file("/home/robert/CLionProjects/untitled/templates/login.html");
}