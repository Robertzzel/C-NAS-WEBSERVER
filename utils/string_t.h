//
// Created by robert on 7/16/24.
//

#ifndef UNTITLED_STRING_T_H
#define UNTITLED_STRING_T_H
#include "inttypes.h"
#include "utils.h"
#include "string_t.h"
#include <string.h>
#include "../functional_utils/vector.h"

void string__to_lower(LIST(char) str);
int string__is_empty(LIST(char) str);
void string__trim_whitespace(LIST(char) this);
void string__substring(LIST(char) this, size_t start_index, size_t end_index);
LIST(char)* string__concatenate_strings(LIST(list_char));
LIST(list_char)* string__split(LIST(char)* this, LIST(char)* delimiter);
LIST(char)* string__join(LIST(list_char)* list, LIST(char)* delim);

#endif //UNTITLED_STRING_T_H
