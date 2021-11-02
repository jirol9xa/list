#ifndef LIST_H
    #define LIST_H

    typedef int type_t;

    struct List
    {
        type_t *data;
        int    *next;
        int    tail;
        int    head;
        int    capacity;
        int    status;
    };

    /*!
        \brief  Функция создания списка
        \param  [List *]list Указатель на список
        \param  [int]capacity Число эелментов
                в списке
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listCtor(List *list, int capacity);
    /*!
        \brief  Функция поиска свободной 
                ячейки в списке
        \param  [List *]list Указатель на списко
        \return 0 в случае успеха, 2 в противном 
                случае
    */
    int searchFree(List *list);
    /*!
        \brief  Функция добавление элемента
                в конец списка
        \param  [List *]list Указатель на список
        \param  [type_t]value Элемент, который
                нужно добавить
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPushBack(List *list, type_t value);
    /*!
        \brief  Функция добавления элемента в 
                произвольное место списка
        \param  [List *]list указатель на список
        \param  [type_t]value Элемент, который
                нужно добавить
        \param  [int]place Номер элемента, полсе
                которого нужно добавить наш
        \return 0 в случае успеха, 1 в противном случае
    */
    int listInsert(List* list, type_t value, int place);
    /*!
        \brief  Функция извлечения элемента с 
                конца списка
        \param  [List *]list Указатель на список
        \param  [tyoe_t *]dest Указатель на переменную,
                куда надо извлечь элемент
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPopBack(List *list, type_t *dest);
     /*!
        \brief  Функция извлечения элемента с 
                начала списка
        \param  [List *]list Указатель на список
        \param  [tyoe_t *]dest Указатель на переменную,
                куда надо извлечь элемент
        \return 0 в случае успеха, 1 в противном
                случае
    */
    int listPopFront(List *list, type_t *dest);

    /*!
        \brief  Функция проверки списка на 
                корректность
        \param  [List *]list Указатель на список
    */
    void verifyList(List *list);
    /*!
        \brief  Функция вывода информации о
                списке
        \param  [List *]list Указатель на список
        \return 0 в случае успеха, 1 в противном
                случае
    */
    void listDump(List *list);


    enum STATUS_AND_ERRS
    {
        FULL_LIST      = 1 << 0,
        EMPTY_LIST     = 1 << 1,
    };


    #define PRINT_RESHETKA(logs) fprintf(logs, "#################################################################\n");
    #define PRINT_LINE() printf("[%s:%d]\n", __func__, __LINE__);
#endif