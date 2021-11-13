#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "logsLib.h"
#include "List.h"
#include <assert.h>


const int THRESHOLD_OF_LIN = 1 << 10; //threshold_of_lin


static int compare(const void *first_elem, const void *second_elem);
static int listLinearizationMem(List *list);
static int listLinearizationQSORT(List *list);
static void printStatus(List *list);
static void printError(List *list);
static unsigned int MurmurHash2(char* key, unsigned int len);
static unsigned int hashCalc(List *list);


static int listCMP(List *standart, List *tested, int list_num);


#define TO_STR(arg) #arg


int listCtor(List *list, int capacity)
{
    assert(list);
    assert(capacity >= 0);

    list->capacity  = capacity + !capacity * 2 + 1;
    list->size      = 1;    //for null element

    list->array     = (elem_t *) calloc(list->capacity, sizeof(elem_t));
    list->tail      = 0;
    list->head      = 0;
    list->free_head = 1;

    list->array[0].prev = 0;
    list->array[0].next = 0;
    list->array[0].value = 0;
    for (int i = 1; i < capacity - 1; i++)
    {
        list->array[i].next = i + 1;
        list->array[i].prev = -1;
    }
    
    list->array[capacity - 1 + !capacity * 2].next = 0;
    list->array[capacity - 1 + !capacity * 2].prev = -1;

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

    if (list->status & FULL_LIST)
    {
        listResize(list, 1);
    }
    openLogs("LOGS/logs");

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;
    list->size ++;
    list->array[free].value = value;
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
    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


int listInsertAfter(List* list, type_t value, int place)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    if (list->status & FULL_LIST)
    {
        listResize(list, 1);
    }

    openLogs("LOGS/logs");

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;
    list->size ++;

    list->array[free].value  = value;
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

    list->status &= (~LINEARIZATED);

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
    for (int i = list->free_head; i != 0; i = list->array[i].next)
    {
        if (list->array[i].prev != -1)
        {
            list->status |= EMPTY_ELEM_ERROR;
            break;
        }
    }
    for (int i = list->array[list->head].next; i != 0; i = list->array[i].next)
    {
        int prev = list->array[i].prev;
        if (list->array[prev].next != i)
        {
            list->status |= DISJOINTED_LIST;
            break;
        }
    }
    for (int i = list->head; i < list->size; i++)
    {
        if (list->array[i].prev == -1) {
            list->status &= (~LINEARIZATED);
            return 0;    
        }
    }

    list->status |= LINEARIZATED;

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
        writeLogs(" %3d", list->array[i].value);
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
    while (head != 0)
    {
        writeLogs(" %3d", list->array[head].value);
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

    writeLogs("    elem_0[color = blue shape = record, label = \" <ind0> index = 0 | <data0> value = 0 " 
                  "| <next0> next = 0 | <prev0> prev = 0 \" ];\n");

    for (int i = 1; i < list->capacity; i++)
    {
        writeLogs("    elem_%d[color = %s shape = record, label = \" <ind%d> index = %d | <value%d>" 
                  "value = %d | <next%d> next = %d | <prev%d> prev = %d \" ];\n", i, (list->array[i].prev == -1) ? "green" : "red", 
                  i, i, i, list->array[i].value, i, list->array[i].next, i, list->array[i].prev);
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

    //system("dot -T png LOGS/GraphLogs.dot -o pic.png");
    //system("eog pic.png");

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
    closeLogs();
    if (list->capacity > list->size * 2)
    {
        listResize(list, 0);
    }
    openLogs("LOGS/logs");


    memcpy(dest, &(list->array[list->head].value), sizeof(type_t));  
    list->size --;  

    int prev                          = list->array[list->tail].prev;
    list->array[prev].next            = 0;
    list->array[list->tail].prev      = -1;
    list->array[list->tail].next      = list->free_head;
    list->free_head                   = list->tail;
    list->tail                        = prev;

    closeLogs();

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
    closeLogs();
    if (list->capacity > list->size * 2)
    {
        listResize(list, 0);
    }

    openLogs("LOGS/logs");

    memcpy(dest, &(list->array[list->head].value), sizeof(type_t));
    list->size --;
    
    int next_elem                     = list->array[list->head].next;
    list->array[next_elem].prev       = 0;
    list->array[list->head].next      = list->free_head;
    list->free_head                   = list->head;
    list->array[list->free_head].prev = -1;
    list->head                        = next_elem;

    closeLogs();

    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


static void printStatus(List *list)
{
    assert(list);

    openLogs("LOGS/logs");
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
    closeLogs();
}


static void printError(List *list)
{
    openLogs("LOGS/logs");
    if (list->status & EMPTY_ELEM_ERROR)
    {
        writeLogs("!!! ERR THE PREVIOUS ELEMENT FOR AN EMPTY ONE IS NOT EQUAL TO -1 !!!");
    }
    if (list->status & DISJOINTED_LIST)
    {
        writeLogs("!!! ERROR LIST IS DISJOINTED !!!\n");
    }
    if (list->status & NOT_RESIZED)
    {
        writeLogs("!!! MEMEMOTY WASN'T ALLOCATTED !!!\n");
    }
    closeLogs();
}


int listInsertBefore(List* list, type_t value, int place)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    if (list->status & FULL_LIST)
    {
        listResize(list, 1);
    }

    openLogs("LOGS/logs");

    int free = list->free_head;
    list->free_head = list->array[list->free_head].next;
    list->size ++;

    list->array[free].value                   = value;
    list->array[free].next                   = place;
    list->array[free].prev                   = list->array[place].prev;
    list->array[place].prev                  = free;
    list->array[list->array[free].prev].next = free;

    if (list->head == place)
    {
        list->head = free;
    }

    closeLogs();

    list->status &= (~LINEARIZATED);

    LIST_DUMP(list);
    ASSERT_OK(list);

    return 0;
}


static int listLinearizationMem(List *list)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    List list_buff = {};
    listCtor(&list_buff, list->capacity);

    for (int i = list->head; i != 0; i = list->array[i].next)
    {
        listPushBack(&list_buff, list->array[i].value);
    }

    free(list->array);

    memcpy(list, &list_buff, sizeof(List));

    LIST_DUMP(list);
    ASSERT_OK(list);


    return 0;    
}


int listResize(List *list, int is_upper)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    if (is_upper)
    {
        void *temp_ptr = nullptr;
        temp_ptr = realloc(list->array, list->capacity * 2 * sizeof(elem_t));
        if (!temp_ptr)
        {
            openLogs("LOGS/logs");
            writeLogs("!!! ERROR Can't allocate memory !!!\n");
            closeLogs();
            return -1;
        }
        list->array = (elem_t *) temp_ptr;
        for (int i = list->capacity; i < list->capacity * 2 - 1; i++)
        {
            list->array[i].value = 0;
            list->array[i].next  = i + 1;
            list->array[i].prev  = -1;
        }
        list->array[list->capacity * 2 - 1].value = 0;
        list->array[list->capacity * 2 - 1].next  = 0;
        list->array[list->capacity * 2 - 1].prev  = -1;


        list->free_head = list->capacity;
        list->capacity *= 2;
    }
    else
    {
        listLinearizationMem(list);
        list->array = (elem_t *) realloc(list->array, list->capacity / 2 * sizeof(elem_t));
        list->capacity /= 2;

        if (list->array[list->capacity - 1].prev == -1)
        {
            list->array[list->capacity - 1].next = 0;
        }

    }

    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


int listRemove(List *list, int index, type_t *dest)
{
    assert(list);
    LIST_DUMP(list);
    ASSERT_OK(list);

    if (index > list->capacity)
    {
        openLogs("LOGS/logs");
        writeLogs("!!! ERROR Index is greater than capacity !!!\n");
        closeLogs();
        return -1;
    }
    if (list->array[index].prev == -1 || index == 0)
    {
        openLogs("LOGS/logs");
        writeLogs("!!! ERROR Can't remove empty element !!!\n");
        closeLogs();
        return -1;
    }
    if (list->status & EMPTY_LIST)
    {
        openLogs("LOGS/logs");
        writeLogs("!!! ERROR Can't pop empty list !!!\n");
        closeLogs();
        return -1;
    }

    memcpy(dest, &(list->array[index].value), sizeof(type_t));
    list->size --;

    int next = list->array[index].next;
    int prev = list->array[index].prev;

    if (index == list->head)
    {
        list->head              = list->array[index].next;
        list->array[next].prev  = 0;
        list->array[index].next = list->free_head;
        list->array[index].prev = -1;
        list->free_head         = index;

        return 0;
    }
    if (index == list->tail)
    {
        list->tail              = list->array[index].prev;
        list->array[prev].next  = 0;
        list->array[index].prev = -1;
        list->array[index].next = list->free_head;
        list->free_head         = index;

        return 0;
    }

    list->array[next].prev  = prev;
    list->array[prev].next  = next;

    list->array[index].prev = -1;
    list->array[index].next = list->free_head;
    list->free_head         = index;

    list->status &= (~LINEARIZATED);

    LIST_DUMP(list);
    ASSERT_OK(list);
    return 0;
}


static int compare(const void *first_elem, const void *second_elem)
{
    assert(first_elem);
    assert(second_elem);

    const elem_t *first  = (const elem_t *) first_elem;
    const elem_t *second = (const elem_t *) second_elem;
    size_t idx1 = first->prev;
    size_t idx2 = second->prev;


    if (idx1 > idx2)  return 1;
    else    return -1; 
}


static int listLinearizationQSORT(List *list)
{
    assert(list);
    qsort(list->array + sizeof(elem_t), list->capacity - 1, sizeof(elem_t), compare);
    listGraphDump(list);
    for (int i = 1; i < list->capacity; i++)
    {
        list->array[i].next = i + 1;
        if (i < list->size)
        {
            list->array[i].prev = i - 1;
        }
        else
        {
            list->array[i].prev = -1;
        }

    }
    list->array[list->capacity - 1].next = 0;
    list->array[list->size - 1].next     = 0;
     
    listGraphDump(list);

    return 0;
}


int listLinearization(List *list)
{
    assert(list);

    if (list->capacity >= THRESHOLD_OF_LIN)
    {
        listLinearizationQSORT(list);
        list->status |= LINEARIZATED;
        return 0;
    }
    
    list->status |= LINEARIZATED;
    listLinearizationMem(list);
    return 0;
}


int findPlace(List *list, int index)
{
    assert(list);
    if (index >= list->size || index <= 0) return -1;

    if (list->status & LINEARIZATED)
    {
        return list->head + index - 1;
    }

    int k = 1;
    for (int i = 1; i < list->size; i++)
    {
        if (i == index) return k;
        k = list->array[k].next;
    }

    return -1;
}


static unsigned int MurmurHash2(char* key, unsigned int len) 
{
    const unsigned int m = 0x5BD1E995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*) key;
    unsigned int k = 0;

    while (len >= 4) {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };
    
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}


static unsigned int hashCalc(List *list)
{ 
    //rall ----- >hash_2 ^ (hash_1 >> 31 | hash_2 << 1)
    unsigned int hash_1 = MurmurHash2((char *) list + sizeof(elem_t *), sizeof(List) - sizeof(elem_t *));
    unsigned int hash_2 = MurmurHash2((char *) list->array, sizeof(elem_t) * list->capacity);
    return hash_2 ^ (hash_1 >> (sizeof(unsigned int) - 1) | hash_2 << 1);
}

int unitTest()
{
    List lst = {};
    listCtor(&lst, 0);
    List LIST1 = {};
    listCtor(&LIST1, 1);
    LIST1.array[0].value = 2;
    LIST1.array[0].next  = 0;
    LIST1.array[0].prev  = 0;
    LIST1.capacity       = 2;
    LIST1.size           = 1;

    listPushBack(&lst, 2);
    
    if (listCMP(&LIST1, &lst, 1))
    {
        openLogs("LOGS/logs");
        writeLogs("UnitTest №1 was failed, programm stopped\n");
        closeLogs();
        listDtor(&lst);
        listDtor(&LIST1);
        return -1;
    }


    listDtor(&lst);
    listDtor(&LIST1);

    return 0;
}


static int listCMP(List *standart, List *tested, int list_num)
{
    assert(standart);
    assert(tested);

    if (standart->capacity != tested->capacity || standart->free_head != tested->free_head
        || standart->head != tested->head || standart->size != tested->size || standart->status != tested->status
        || standart->tail != tested->tail)
    {
        openLogs("LOGS/logs");
        writeLogs("!!! ERROR Wrong list parameters in unitTest with list number --- %d!!!\n", list_num);
        closeLogs();
        return -1;
    }

    for (int i = 0; i < standart->capacity; i++)
    {
        if (standart->array[i].next != tested->array[i].next || standart->array[i].prev != tested->array[i].prev ||
            standart->array[i].value != tested->array[i].value)
        {
            openLogs("LOGS/logs");
            writeLogs("!!! ERROR in unitTest with list number -- %d\n", list_num);
            writeLogs("!!! On place %d located elem:\nprev = %d\nnext = %d\nvalue = %d\n", i, tested->array[i].prev,
                      tested->array[i].next, tested->array[i].value);
            writeLogs("!!! But expected:\nprev = %d\nnext = %d\nvalue = %d\n", standart->array[i].prev, standart->array[i].next,
                      standart->array[i].value);
            closeLogs();
            return -1;
        }
    }

    return 0;
}