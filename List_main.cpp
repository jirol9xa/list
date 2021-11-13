#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 0);
    type_t mass[20] = {};
    for (int i = 0; i < 10; i++)
    {
        listPushBack(&lst, i + 1);
    } 
    listRemove(&lst, 5, &(mass[0]));   
    listGraphDump(&lst);
    listLinearization(&lst);    
    listGraphDump(&lst);
    printf("%d\n", findPlace(&lst, 3));
    listGraphDump(&lst);
    listDtor(&lst);

    unitTest();

    return 0;
}