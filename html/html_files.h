//
// Created by robert on 7/3/24.
//

#ifndef UNTITLED_HTML_FILES_H
#define UNTITLED_HTML_FILES_H

#include "stdio.h"
#include "../ctemplate/ctemplate.h"
#include "../error.h"
#include "stdlib.h"
#include "string.h"
#include "../file_utils/file_utils.h"

#define HTML_TEMPLATES_DIRECTORY "/home/robert/CLionProjects/untitled/templates/"

error get_login_page(char** page);
error get_home_page(char** page);

#endif //UNTITLED_HTML_FILES_H
