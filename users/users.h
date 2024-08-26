//
// Created by robert on 7/10/24.
//

#ifndef UNTITLED_USERS_H
#define UNTITLED_USERS_H
#include "../error.h"
#include "string.h"
#include <stdio.h>
#include "openssl/sha.h"
#include "stdbool.h"
#define USERS_FILE "../users.txt"
#define LINE_MAX_SIZE 1024

bool check_username_and_password(const char* username, const char* password);

#endif //UNTITLED_USERS_H
