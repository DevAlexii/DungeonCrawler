#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
    int count;
} List;

List* Data_struct_create_list() {
    List* newList = malloc(sizeof(List));
    if (newList != NULL) {
        newList->head = NULL;
        newList->count = 0;
    }
    return newList;
}

Node* Data_struct_list_get_tail(List* list){
    Node* current = list->head;
    Node* tail = NULL;

    while (current){
        tail = current;
        current = current->next;
    }

    return tail;
}

List* Data_struct_list_add(List* list, Node* item) {
    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) {
        list->head = item;
    } else {
        Node* tail = Data_struct_list_get_tail(list);
        tail->next = item;
    }

    list->count++;

    return list;
}