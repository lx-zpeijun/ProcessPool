#ifndef PROCESS_POOL_LIST_H
#define PROCESS_POOL_LIST_H

typedef struct node {
    void *data;
    struct node *previous;
    struct node *next;
} node_t;

typedef struct list {
    int node_count;
    node_t *head;
    node_t *tail;
    pthread_mutex_t mutex;
} list_t;

list_t *list_init();
void list_push(list_t *list, void *data);
void *list_pop_front(list_t *list);
void *list_pop_back(list_t *list);
void list_free_without_data(list_t *list);
void list_free(list_t *list);

#endif //PROCESS_POOL_LIST_H
