//
// Created by robert on 7/24/24.
//

#ifndef UNTITLED_VECTOR_H
#define UNTITLED_VECTOR_H

#include <stdlib.h>
#include "../utils/utils.h"

#define LIST(T) list_##T
#define LIST_DEFINE(T) typedef struct {T* data; size_t size; size_t cap;} list_##T;

#define LIST_NEW(T, arraySize, arrayCap) ({ \
    LIST(T)* r = NULL;\
    if(arrayCap > 0){\
        r = xmalloc(sizeof(LIST(T)));                                    \
        r->data = xmalloc(sizeof(T) * arrayCap);\
        memset(r->data, 0, arraySize);\
        r->size = arraySize;\
        r->cap = arrayCap;\
    }\
    r;\
})

#define LIST_ADD(list, element) do { \
    if((list)->size>=(list)->cap) {            \
        LIST_RESIZE((list), (list)->cap * 2);                                 \
    }                        \
    (list)->data[(list)->size] = (element);\
    (list)->size++;\
}while(0)

#define LIST_GET(list, index) ({ \
    if((list)->size <= (index)) {            \
        exit(2);                                 \
    }                        \
    (list)->data + (index);\
})

#define LIST_REMOVE_END(list, nr) do { \
    if((list)->size >= nr) {            \
        (list)->size -= nr;                                 \
    }                        \
}while(0)

#define LIST_ADD_MULTIPLE(list, elements, number_of_elements) do { \
    size_t new_size = list->size + number_of_elements;                                                               \
    if(new_size>=r->cap) {                                         \
        size_t new_cap = list->cap;\
        while(new_size>=new_cap) {                                  \
            new_cap *= 2;                                                           \
        }\
        LIST_RESIZE(list, new_cap);                                 \
    }                        \
    memmove(list->data + list->size, elements, number_of_elements);                                                               \
    list->size+=number_of_elements;\
}while(0)

#define LIST_RESIZE(list, new_size) do { \
    list->data = xrealloc(list->data, new_size); \
    list->cap = new_size;                                         \
}while(0)

#define LIST_FREE(list) do{ \
    free((list)->data);      \
    free(list);\
}while(0)

#define FOR_EACH(list, function) do { \
    for(size_t i = 0; i < list->size; ++i){ \
        function(list->data[i]);\
    }\
}while(0)

#define MAP(list, function, return_type) ({\
    LIST(return_type)* output_array = 0;\
    if(list != 0 && list->size > 0){\
        output_array = LIST_NEW(return_type, list->size); \
        for (size_t i = 0; i < list->size; ++i) {\
            output_array->data[i] = function(list->data[i]);\
        }\
    }\
    output_array;\
})

#define MAP_WITH_INDEX(list, function, return_type) ({\
    LIST(return_type)* output_array = 0;\
    if(list != 0 && list->size > 0){\
        output_array = LIST_NEW(return_type, list->size); \
        for (size_t i = 0; i < list->size; ++i) {\
            output_array->data[i] = function(list->data[i], i);\
        }\
    }\
    output_array;\
})

#define FILTER(list, function) ({\
    typeof(list) output_array = NULL;\
    if(list != NULL && list->size > 0){\
        output_array = LIST_NEW(typeof(list->data[0]), list->size);\
        unsigned int elements_found = 0;\
        for (size_t i = 0; i < list->size; ++i) {\
            if(function(list->data[i])) {\
                output_array->data[elements_found] = list->data[i];\
                elements_found++;\
            }\
        }\
        if(elements_found > 0){\
            LIST_RESIZE(output_array, elements_found);\
        } else {\
            LIST_FREE(output_array);\
            output_array = 0;\
        }\
    }\
    output_array;\
})

#define FILTER_WITH_INDEX(list, function) ({\
    typeof(list)* output_array = NULL;\
    if(list != NULL && list->size > 0){\
        output_array = LIST_NEW(typeof(list->data[0]), list->size);\
        unsigned int elements_found = 0;\
        for (size_t i = 0; i < list->size; ++i) {\
            if(function(list->data[i], i)) {\
                output_array->data[elements_found] = list->data[i];\
                elements_found++;\
            }\
        }\
        if(elements_found > 0){\
            LIST_RESIZE(output_array, elements_found);\
        } else {\
            LIST_FREE(output_array);\
            output_array = 0;\
        }\
    }\
    output_array;\
})

#define REDUCE(list, function, initial_value) ({\
    typeof(initial_value) result = initial_value;\
    if(list != NULL && list->size > 0){\
        for (size_t i = 0; i < list->size; ++i) {\
            result = function(result, list->data[i]);\
        }\
    }\
    result;\
})

LIST_DEFINE(short)
LIST_DEFINE(int)
LIST_DEFINE(long)
LIST_DEFINE(char)
LIST_DEFINE(float)
LIST_DEFINE(double)

LIST_DEFINE(list_char)

//typedef struct {
//    char *data;
//    size_t size;
//    size_t cap;
//} list_char;
//
//typedef struct {
//    list_char *data;
//    size_t size;
//    size_t cap;
//} list_list_char;

#endif //UNTITLED_VECTOR_H
