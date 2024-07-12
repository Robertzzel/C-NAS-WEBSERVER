//
// Created by robert on 7/10/24.
//


#include "users.h"

bool check_username_and_password(const char* username, const char* password) {
    FILE *f = fopen(USERS_FILE, "rb");
    if(f == NULL){
        return false;
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password, strlen(password));
    SHA256_Final(hash, &sha256);

    char sha256Hex[65];
    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(sha256Hex + (i * 2), "%02x", hash[i]);
    }
    sha256Hex[64] = 0;

    char line[LINE_MAX_SIZE];
    while(fgets(line, LINE_MAX_SIZE, f) != NULL) {
        line[strlen(line)- 1] = 0;
        char* delimiter = strchr(line, ',');
        if(delimiter == NULL){
            fclose(f);
            return false;
        }
        char* password_hash = delimiter + 1;
        int username_matches = strncmp(username, line, delimiter - line) == 0;
        int password_matches = strncmp(password_hash, sha256Hex, SHA256_DIGEST_LENGTH) == 0;
        if(username_matches && password_matches){
            fclose(f);
            return true;
        }
    }

    fclose(f);
    return false;
}