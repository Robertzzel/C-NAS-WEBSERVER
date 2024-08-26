#include "html_files.h"

char* get_home_page(list_file_t* files) {
    char* file_contents = read_file("/home/robert/CLionProjects/untitled/templates/home.html");
    if(file_contents == NULL){
        return NULL;
    }

    char* tag_to_replace_with_files = "`@files`";
    char* index_of_tag = strstr(file_contents, tag_to_replace_with_files);
    if(index_of_tag == NULL){
        return file_contents;
    }

    char* tag_replacement = xcalloc(1);
    for (int i = 0; i < files->size; ++i) {
        file_t* file = list_file_t__get(files, i);
        char* file_json = list_file_t__to_json(file);
        tag_replacement = xrealloc(tag_replacement, strlen(tag_replacement) + strlen(file_json) + 2);
        strcat(tag_replacement, file_json);
        strcat(tag_replacement, ",");
        free(file_json);
    }

    uint32_t size_of_file_before_tag = index_of_tag - file_contents;
    uint32_t size_of_file_after_tag = strlen(index_of_tag + strlen(tag_to_replace_with_files));
    size_t resultLength = size_of_file_before_tag + strlen(tag_replacement) + size_of_file_after_tag;

    char* result = xcalloc(resultLength + 1);
    strncat(result, file_contents, size_of_file_before_tag);
    strcat(result, tag_replacement);
    strcat(result, index_of_tag + strlen(tag_to_replace_with_files));

    free(file_contents);
    free(tag_replacement);
    return result;
}

char* get_login_page() {
    return read_file("/home/robert/CLionProjects/untitled/templates/login.html");
}