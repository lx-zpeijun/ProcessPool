#include <pthread.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

list_t *list_init() {
    list_t *list = calloc(1, sizeof(list_t));
    if(pthread_mutex_init(&list->mutex, NULL) != 0) {
        printf("Error creating mutex. \n");
        exit(1);
    }
    return list;
}

void list_push(list_t *list, void *data) {
    pthread_mutex_lock(&list->mutex);
    node_t *element = calloc(1, sizeof(node_t));

    element->data = data;
    if (!list->head) { //还未有数据节点
        list->head = element;
        list->tail = element;
        list->tail->next = element;
        list->tail->previous = element;
        list->head->next = element;
        list->head->previous = element;
        list->node_count += 1;
        pthread_mutex_unlock(&list->mutex);
        return;
    }

    list->tail->next = element;
    element->previous = list->tail;
    list->tail = element;
    element->next = list->head;
    list->head->previous = list->tail;
    list->node_count += 1;
    pthread_mutex_unlock(&list->mutex);
}

void *list_pop_front(list_t *list) {
    if (list->head == NULL) {
        return NULL;
    }
    pthread_mutex_lock(&list->mutex);

    node_t *node = list->head;
    node_t *next = list->head->next;

    if(next == node) {
        list->head = NULL;
        list->tail = NULL;
        goto end;
    }

    list->head = next;
    list->tail->next = next;
    next->previous = list->tail;

    end:
    list->node_count -= 1;
    pthread_mutex_unlock(&list->mutex);
    void *data = node->data;
    free(node);
    return data;
}

//从双向循环链表的尾部移除一个节点，并返回该节点中存储的数据。
void *list_pop_back(list_t *list) {
    if (list->tail == NULL) {
        return NULL;
    }
    pthread_mutex_lock(&list->mutex);

    node_t *node = list->tail;
    node_t *previous = list->tail->previous;

    if(previous == node) {
        list->head = NULL;
        list->tail = NULL;
        goto end;
    }

    list->tail = previous;
    list->head->previous = previous;
    previous->next = list->head;

    end:
    list->node_count -= 1;
    pthread_mutex_unlock(&list->mutex);
    void *data = node->data;
    free(node);
    return data;
}


void _list_free(list_t *list, int with_data) {
    node_t *node = list->head;
    if (node) {
        do {
            node_t *to_free = node;
            node = node->next;
            if (with_data)
                free(to_free->data);
            free(to_free);
        } while (node != list->head);
    }
    if(pthread_mutex_destroy(&list->mutex) != 0) {
        printf("Error destroying mutex. \n");
        exit(1);
    }
    free(list);
}

void list_free_without_data(list_t *list) {
    _list_free(list, 0);
}

void list_free(list_t *list) {
    _list_free(list, 1);
}
