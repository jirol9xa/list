#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "logsLib.h"
#include "List.h"
#include <assert.h>


#define NAME_ELEM(arg) elem_##arg;
#define TO_STR(arg) #arg;


int listCtor(List *list, int capacity)
{
    assert(list);
    assert(capacity > 1);

    list->capacity = capacity;

    list->array     = (elem *) calloc(capacity, sizeof(elem));
    list->tail      = 0;
    list->head      = 0;
    list->free_head = 1;

    list->array[0].prev = 0;
    list->array[0].next = 0;
    list->array[0].data = 0;
    for (int i = 1; i < capacity - 1; i++)
    {
        list->array[i].next = i + 1;
        list->array[i].prev = -1;
    }
    if (capacity > 1)
    {
        list->array[capacity - 1].next = 0;
        list->array[capacity - 1].prev = -1;
    }

    listGraphDump(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    return 0;
}


int listDtor(List *list)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    free(list->array);
    return 0;
}


int listPushBack(List *list, type_t value)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    openLogs("LOGS/logs");

    if (list->status & FULL_LIST)
    {
        writeLogs("!!!! ERROR: Can't push in full list !!!! \n");
        return FULL_LIST;
    }

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;
    
    list->array[free].data = value;
    if (list->tail != 0)
    {
        list->array[list->tail].next = free;
    }
    list->array[free].next = 0;
    list->array[free].prev = list->tail;
    
    if (list->tail == list->head && list->head == 0)
    {
        list->head = free;
    }
    list->tail = free;

    closeLogs();
    listGraphDump(list);
    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


int listInsertAfter(List* list, type_t value, int place)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    openLogs("LOGS/logs");

    if (list->status & FULL_LIST)
    {
        writeLogs("!!!! ERROR: Can't push in full list !!!! \n");
        return FULL_LIST;
    }

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;

    list->array[free].data  = value;
    list->array[free].next  = list->array[place].next;
    list->array[free].prev  = place;
    list->array[list->array[free].next].prev = free;
    list->array[place].next = free;
    
    if (list->tail == place)
    {
        list->tail = free;
    }
    else if (list->head == list->tail && list->head == 0)
    {
        list->head = free;
        list->tail = free;
    }

    closeLogs();

    LIST_DUMP(list);
    ASSERT_OK(list);

    return 0;
}


int verifyList(List *list)
{   
    assert(list);

    list->status = 0;

    bool is_full = (list->free_head == 0);
    list->status |= is_full;

    if (list->tail == 0)
    {
        list->status |= EMPTY_LIST;
    }

    for (int i = list->free_head; i != 0;)
    {
        if (list->array[i].prev != -1)
        {
            list->status |= EMPTY_ELEM_ERROR;
        }
        i = list->array[i].next;
    }
    for (int i = list->array[list->head].next; i != 0;)
    {
        int prev = list->array[i].prev;
        if (list->array[prev].next != i)
        {
            list->status |= DISJOINTED_LIST;
        }
        i = list->array[i].next;
    }

    return 0;
}


void listTextDump(List *list)
{
    assert(list);

    openLogs("LOGS/logs");

    printReshetka();
    writeLogs("PLACE");
    for (int i = 0; i < list->capacity; i++)
    {
        writeLogs(" %3d", i);
    }
    writeLogs("\n");
    writeLogs("DATA ");
    for (int i = 0; i < list->capacity; i++)
    {
        writeLogs(" %3d", list->array[i].data);
    }
    writeLogs("\n");
    writeLogs("NEXT ");
    for (int i = 0; i < list->capacity; i++)
    {
        writeLogs(" %3d", list->array[i].next);
    }
    writeLogs("\n");
    writeLogs("PREV ");
    for (int i = 0; i < list->capacity; i++)
    {
        writeLogs(" %3d", list->array[i].prev);
    }
    writeLogs("\n");

    writeLogs("LOGIC");
    int head = list->head;
    int tail = list->tail;
    while (head != 0)
    {
        writeLogs(" %3d", list->array[head].data);
        head = list->array[head].next;
    }

    writeLogs("\n");

    writeLogs("   HEAD = %d\n   TAIL = %d\n\n", list->head, list->tail);

    writeLogs("list status = %d\nfree_head = %d\n", list->status, list->free_head);
    printStatus(list);
    printReshetka();
    writeLogs("\n");

    closeLogs();
}


void listGraphDump(List *list)
{
    assert(list);

    FFFFFree("LOGS/GraphLogs.dot");

    openLogs("LOGS/GraphLogs.dot");

    writeLogs("digraph G{\n    rankdir = LR\n");

    writeLogs("    elem_0[color = blue shape = record, label = \" <ind0> index = 0 | <data0> data = 0 " 
                  "| <next0> next = 0 | <prev0> prev = 0 \" ];\n");

    for (int i = 1; i < list->capacity; i++)
    {
        writeLogs("    elem_%d[color = %s shape = record, label = \" <ind%d> index = %d | <data%d>" 
                  "data = %d | <next%d> next = %d | <prev%d> prev = %d \" ];\n", i, (list->array[i].prev == -1) ? "green" : "red", 
                  i, i, i, list->array[i].data, i, list->array[i].next, i, list->array[i].prev);
        writeLogs("    elem_%d -> elem_%d[style = invis]", i - 1, i);
    }

    writeLogs("    elem_0:<next0> -> elem_0:<prev0>;\n");
    writeLogs("    elem_0:<prev0> -> elem_0:<next0>;\n");

    for (int i = 1; i < list->capacity; i++)
    {
        if (list->array[i].prev == -1 && list->array[i].next != 0)
        {
            writeLogs("    elem_%d:<next%d> -> elem_%d:<next%d>[constraint = false, color = green];\n", 
                      i, i, list->array[i].next, list->array[i].next);
        }
        else if (list->array[i].prev != -1)
        {
            writeLogs("    elem_%d:<next%d> -> elem_%d:<prev%d>[constraint = false, color = red];\n",
                      i, i, list->array[i].next, list->array[i].next);
        }
    }

    writeLogs("}\n");

    closeLogs();

    system("dot -T png LOGS/GraphLogs.dot -o pic.png");
    system("eog pic.png");

    FFFFFree("LOGS/GraphLogs.dot");
}


int listPopBack(List *list, type_t *dest)
{
    assert(list);
    assert(dest);
    LIST_DUMP(list);
    ASSERT_OK(list);

    openLogs("LOGS/logs");

    if (list->status & EMPTY_LIST)
    {
        writeLogs("!!!! ERROR: Can't pop empty list !!!! \n");
        return EMPTY_LIST;
    }

    *dest = list->array[list->tail].data;
    
    

    int prev                          = list->array[list->tail].prev;
    list->array[prev].next            = 0;
    list->array[list->tail].prev      = -1;
    list->array[list->tail].next      = list->free_head;
    list->free_head                   = list->tail;
    list->tail                        = prev;

    closeLogs();

    listGraphDump(list);
    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


int listPopFront(List *list, type_t *dest)
{
    assert(list);
    assert(dest);
    LIST_DUMP(list);
    ASSERT_OK(list);

    openLogs("LOGS/logs");

    verifyList(list);
    if (list->status & EMPTY_LIST)
    {
        writeLogs("!!!! ERROR: Can't pop empty list !!!! \n");
        return EMPTY_LIST;
    }

    *dest = list->array[list->head].data;
    
    int next_elem                     = list->array[list->head].next;
    list->array[next_elem].prev       = 0;
    list->array[list->head].next      = list->free_head;
    list->free_head                   = list->head;
    list->array[list->free_head].prev = -1;
    list->head                        = next_elem;

    closeLogs();

    listGraphDump(list);
    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


void printStatus(List *list)
{
    assert(list);

    if (list->status & FULL_LIST)
    {
        writeLogs("LIST IS FULL\n");
    }
    if (list->status & EMPTY_LIST)
    {
        writeLogs("LIST IS EMPTY\n");
    }
    if (list->status & DISJOINTED_LIST)
    {
        writeLogs("!!! ERROR LIST IS DISJOINTED!!!\n");
    }
}


void printError(List *list)
{
    openLogs("LOGS/logs");
    if (list->status & EMPTY_ELEM_ERROR)
    {
        writeLogs("!!! ERR THE PREVIOUS ELEMENT FOR AN EMPTY ONE IS NOT EQUAL TO -1 !!!");
    }
    if (list->status & DISJOINTED_LIST)
    {
        writeLogs("!!! ERROR LIST IS DISJOINTED!!!\n");
    }
    closeLogs();
}


int listInsertBefore(List* list, type_t value, int place)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    openLogs("LOGS/logs");

    if (list->status & FULL_LIST)
    {
        writeLogs("!!!! ERROR: Can't push in full list !!!! \n");
        return FULL_LIST;
    }

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;

    list->array[free].data                   = value;
    list->array[free].next                   = place;
    list->array[free].prev                   = list->array[place].prev;
    list->array[place].prev                  = free;
    list->array[list->array[free].prev].next = free;

    if (list->head == place)
    {
        list->head = free;
    }

    closeLogs();

    LIST_DUMP(list);
    ASSERT_OK(list);

    return 0;
}
