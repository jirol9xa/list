#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 10);
    type_t mass[20] = {};
    listPushBack(&lst, 4);
    listPushBack(&lst, 5);
    listPushBack(&lst, 6);
    listPushBack(&lst, 7);
    listPushBack(&lst, 8);
    listPushBack(&lst, 9);
    listInsertBefore(&lst, 24, 5);
    listInsertAfter(&lst, 25, 5);
    listInsertAfter(&lst, 26, 4);
    listPopBack(&lst, &(mass[0]));
    listPopBack(&lst, &(mass[1]));
    listPopFront(&lst, &(mass[2]));
    listPopFront(&lst, &(mass[3]));

    printf("Back = %d\nBack = %d\nFront = %d\nFront = %d\n", mass[0], mass[1], mass[2], mass[3]);

    return 0;
}