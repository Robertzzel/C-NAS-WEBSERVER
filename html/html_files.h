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

#define HOME_PAGE_FILE_URL "../templates/home.html"

error get_home_page(char** file_content);

#endif //UNTITLED_HTML_FILES_H
