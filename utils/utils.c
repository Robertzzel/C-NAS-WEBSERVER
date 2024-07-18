//
// Created by robert on 6/30/24.
//

#include "utils.h"

void memory_error_and_abort (char *fname)
{
    fprintf (stderr, "%s: out of virtual memory\n", fname);
    exit (2);
}

void* xmalloc(size_t bytes) {
    void *temp = malloc (bytes);
    if (temp == 0)
        memory_error_and_abort ("xmalloc");
    return temp;
}

void* xrealloc (void* pointer, size_t bytes)
{
    void* temp = pointer ? realloc (pointer, bytes) : malloc (bytes);
    if (temp == 0)
        memory_error_and_abort ("xrealloc");
    return temp;
}

void* xcalloc(size_t bytes) {
    void *temp = calloc(1, bytes);
    if (temp == 0)
        memory_error_and_abort ("xmalloc");
    return temp;
}

char* xstrdup(const char* string) {
    char *temp = strdup(string);
    if (temp == 0)
        memory_error_and_abort ("xstrdump");
    return temp;
}

char* xstrndup(const char* string, size_t length) {
    char *temp = strndup(string, length);
    if (temp == 0)
        memory_error_and_abort ("xstrndump");
    return temp;
}