#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 10);
    type_t mass[20] = {};
    PRINT_LINE();
    listPushBack(&lst, 4);
    listPushBack(&lst, 5);

    type_t *b = (type_t *) calloc(1, 1);
    listPopBack(&lst, b);

    listPushBack(&lst, 6);
    listPushBack(&lst, 7);
    listPushBack(&lst, 8);
    listPushBack(&lst, 9);
    listInsert(&lst, 24, 5);
    listInsert(&lst, 25, 5);
    listInsert(&lst, 26, 5);
    listPopBack(&lst, &(mass[0]));
    listPopBack(&lst, &(mass[1]));
    listPopFront(&lst, &(mass[2]));
    listPopFront(&lst, &(mass[3]));

    printf("Back = %d\nBack = %d\nFront = %d\nFront = %d\n", mass[0], mass[1], mass[2], mass[3]);



    // type_t a = {};
    // type_t b = {};

    // listPopBack(&lst, &a);
    // printf("a = %d\n", a);
    // listPopFront(&lst, &b);
    // printf("b = %d\n", b);

    return 0;
}