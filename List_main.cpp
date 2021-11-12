#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "logsLib.h"

int main(void)
{
    List lst = {};
    listCtor(&lst, 0);
    type_t mass[20] = {};
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);    
    listPushBack(&lst, 8);
    listRemove(&lst, 6, &(mass[0]));    
    listRemove(&lst, 7, &(mass[0]));    
    listRemove(&lst, 8, &(mass[0]));    
    listRemove(&lst, 9, &(mass[0]));    
    listRemove(&lst, 10, &(mass[0]));    
    listRemove(&lst, 11, &(mass[0]));
    listGraphDump(&lst);
    listLinearization(&lst);   
    PRINT_LINE(); 
    listGraphDump(&lst);
    listDtor(&lst);

    return 0;
}