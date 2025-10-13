#include <stdio.h>
#include "data.h"

int main(void) {
    int r = validate();
    if (r == 0) {
        printf("VALID\n");
    } else {
        printf("%d\n", r);
    }
    return 0;
}