#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "List.h"

FILE *logs = fopen("logs", "w");

int listCtor(List *list, int capacity)
{
    list->capacity = capacity;

    list->data = (type_t *) calloc(capacity, sizeof(type_t));
    list->next = (int *)    calloc(capacity, sizeof(int));
    list->tail = 0;
    list->head = 0;
    for (int i = 1; i < capacity; i++)
    {
        list->next[i] = -1;
    }
    listDump(list);

    return 0;
}


int searchFree(List *list)
{
    for (int i = 1; i < list->capacity; i++)
    {
        if (list->next[i] == -1) return i;
    }

    return -1;
}


int listPushBack(List *list, type_t value)
{
    verifyList(list);
    if (list->status & FULL_LIST)
    {
        fprintf(logs, "!!!! ERROR: Can't push in full list !!!! \n");
        return FULL_LIST;
    }

    int free = searchFree(list);
    
    list->data[free] = value;
    if (list->tail != 0)
        list->next[list->tail] = free;
    list->next[free] = 0;
    
    if (list->tail == list->head && list->head == 0)
    {
        list->head = free;
    }
    list->tail = free;

    listDump(list);
    return 0;
}


int listInsert(List* list, type_t value, int place)
{
    verifyList(list);
    if (list->status & FULL_LIST)
    {
        fprintf(logs, "!!!! ERROR: Can't push in full list !!!! \n");
        return FULL_LIST;
    }

    int free = searchFree(list);

    list->data[free] = value;
    list->next[free] = list->next[place];
    list->next[place] = free;
    
    if (list->tail == place)
    {
        list->tail = free;
    }
    else if (list->head == list->tail && list->head == 0)
    {
        list->head = free;
        list->tail = free;
    }

    listDump(list);

    return 0;
}


void verifyList(List *list)
{   
    list->status = 0;

    bool is_full = 1;
    for (int i = 1; i < list->capacity; i++)
    {
        if (list->next[i] == -1)
        {
            is_full = 0;
        }
    }
    list->status |= is_full;

    if (list->tail == 0)
    {
        list->status |= EMPTY_LIST;
    }
}


void listDump(List *list)
{
    PRINT_RESHETKA(logs);
    fprintf(logs, "PLACE");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(logs, " %3d", i);
    }
    fprintf(logs, "\n");
    fprintf(logs, "DATA ");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(logs, " %3d", list->data[i]);
    }
    fprintf(logs, "\n");
    fprintf(logs, "NEXT ");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(logs, " %3d", list->next[i]);
    }
    fprintf(logs, "\n");

    fprintf(logs, "LOGIC");
    int head = list->head;
    int tail = list->tail;
    while(head != 0)
    {
        fprintf(logs, " %3d", list->data[head]);
        head = list->next[head];
    }
    fprintf(logs, "\n");

    fprintf(logs, "   HEAD = %d\n   TAIL = %d\n", list->head, list->tail);
    fprintf(logs, "\n");

    fprintf(logs, "list status = %d\n", list->status);

    PRINT_RESHETKA(logs);
    fprintf(logs, "\n");
}


int listPopBack(List *list, type_t *dest)
{
    verifyList(list);
    if (list->status & EMPTY_LIST)
    {
        fprintf(logs, "!!!! ERROR: Can't pop empty list !!!! \n");
        return EMPTY_LIST;
    }

    *dest = list->data[list->tail];
    
    int previous = 0;
    for (int i = 1; i < list->capacity; i++)
    {
        if (list->next[i] == list->tail)
        {
            previous = i;
        }
    }
    list->next[list->tail] = -1;
    list->tail             = previous;
    list->next[previous]   = 0;

    listDump(list);
    return 0;
}


int listPopFront(List *list, type_t *dest)
{
    verifyList(list);
    if (list->status & EMPTY_LIST)
    {
        fprintf(logs, "!!!! ERROR: Can't pop empty list !!!! \n");
        return EMPTY_LIST;
    }

    *dest = list->data[list->head];
    
    int next_elem          = list->next[list->head];
    list->next[list->head] = -1;
    list->head             = next_elem;

    listDump(list);
    return 0;
}