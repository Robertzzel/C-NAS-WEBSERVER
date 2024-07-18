//
// Created by robert on 7/3/24.
//

#ifndef UNTITLED_HTML_FILES_H
#define UNTITLED_HTML_FILES_H

#include "stdio.h"
#include "../ctemplate/ctemplate.h"
#include "../error.h"
#include "stdlib.h"
#include "../utils/file_utils.h"
#include "../utils/file.h"

#define HTML_TEMPLATES_DIRECTORY "/home/robert/CLionProjects/untitled/templates/"

char* get_login_page();
char* get_home_page(list_file_t* files);

#endif //UNTITLED_HTML_FILES_H
