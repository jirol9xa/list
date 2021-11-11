#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 0);
    type_t mass[20] = {};
    listPushBack(&lst, 4);
    printf("cap = %d, size = %d\n", lst.capacity, lst.size);
    listPushBack(&lst, 5);
    listPushBack(&lst, 6);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listPushBack(&lst, 7);
    listInsertBefore(&lst, 24, 3);
    listInsertAfter(&lst, 25, 2);
    listInsertAfter(&lst, 26, 4);
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[0]));
    listPopFront(&lst, &(mass[1]));
    listLinearization(&lst);

    printf("Back = %d\nBack = %d\n", mass[0], mass[1]);

    listDtor(&lst);

    return 0;
}