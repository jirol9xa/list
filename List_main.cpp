#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 0);
    type_t mass[20] = {};
    listLinearization(&lst);

    printf("Back = %d\nBack = %d\n", mass[0], mass[1]);

    listDtor(&lst);

    return 0;
}