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

error write_home_page_to_socket(socket_t* socket);
error get_home_page(char** page);

#endif //UNTITLED_HTML_FILES_H
