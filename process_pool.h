#ifndef PROCESS_POOL_PROCESS_POOL_H
#define PROCESS_POOL_PROCESS_POOL_H

#include "list.h"

#define task_output_list_t list_t
#define task_list_t list_t

typedef struct task_output {
    int number;
    int is_prime;
} task_output_t;

typedef struct task {
    task_output_t (*function)(void *);
    void *argument;
} task_t;

typedef struct pipes {
    int read_pipe_fd;
    int write_pipe_fd;
} pipes_t;

typedef struct process_pool {
    int process_count;
    pipes_t *processes_pipes;
    int task_count;
    int done_count;
} process_pool_t;

process_pool_t *process_pool_init(int process_count);
void process_pool_map(process_pool_t *pool, task_list_t *input_list, task_output_list_t *output_list);
void process_pool_free(process_pool_t *pool);

#endif //PROCESS_POOL_PROCESS_POOL_H
