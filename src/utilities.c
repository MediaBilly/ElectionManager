#include <stdio.h>
#include <stdlib.h>
#include "../headers/utilities.h"

void not_enough_memory() {
    printf("Not enough memory\n");
}

void DestroyString(string *str) {
    if (*str != NULL) {
        free(*str);
        *str = NULL;
    }
}
