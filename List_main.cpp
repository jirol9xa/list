#include <stdio.h>
#include <stdlib.h>
#include "List.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 8);
    listPushBack(&lst, 4);

    type_t a = {};
    type_t b = {};

    listPopBack(&lst, &a);
    printf("a = %d\n", a);
    listPopFront(&lst, &b);
    printf("b = %d\n", b);

    return 0;
}