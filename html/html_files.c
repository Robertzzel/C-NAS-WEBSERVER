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

    char* tag_replacement = string__new(0);
    for (int i = 0; i < files->size; ++i) {
        file_t* file = list_file_t__get(files, i);
        char* file_json = list_file_t__to_json(file);
        char* file_json_with_ending_comma = string__append(file_json, ",");
        tag_replacement = string__append(tag_replacement, file_json_with_ending_comma);
        free(file_json);
        free(file_json_with_ending_comma);
    }

    uint32_t size_of_file_before_tag = index_of_tag - file_contents;
    uint32_t size_of_file_after_tag = strlen(index_of_tag + strlen(tag_to_replace_with_files));
    size_t resultLength = size_of_file_before_tag + strlen(tag_replacement) + size_of_file_after_tag;

    char* result = string__new(resultLength);
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