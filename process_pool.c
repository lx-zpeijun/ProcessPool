#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "list.h"
#include "process_pool.h"

void process_loop(pipes_t *pipes) {
    task_t task;
    task_output_t result;

    while (read(pipes->read_pipe_fd, &task, sizeof(task)) > 0) {
        printf("Perform task = %d by pid = %d\n", task.argument, getpid());
        result = task.function(task.argument);
        if (write(pipes->write_pipe_fd, &result, sizeof(result)) == -1) {
            printf("Error writing result to pipe.\n");
            exit(1);
        }
    }

    close(pipes->read_pipe_fd);
    close(pipes->write_pipe_fd);
    exit(0);
}


process_pool_t *process_pool_init(int process_count) {
    process_pool_t *pool = calloc(1, sizeof(process_pool_t));
    pool->process_count = process_count;
    pool->processes_pipes = calloc(process_count, sizeof(pipes_t));


    for (int i = 0; i < process_count; ++i) {
        int pipe_child_parent[2];
        int pipe_parent_child[2];

        if (pipe(pipe_child_parent) == -1 ||
            pipe(pipe_parent_child) == -1) {
            printf("Error creating pipe.\n");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == -1) {
            printf("Error forking.\n");
            exit(1);
        }

        if (pid == 0) {
            // child process
            close(pipe_child_parent[0]);
            close(pipe_parent_child[1]);
            pipes_t pipes = {.read_pipe_fd =pipe_parent_child[0], .write_pipe_fd =pipe_child_parent[1]};
            process_loop(&pipes);
        }
        //parent process
        close(pipe_child_parent[1]);
        close(pipe_parent_child[0]);

        pool->processes_pipes[i].read_pipe_fd = pipe_child_parent[0];
        pool->processes_pipes[i].write_pipe_fd = pipe_parent_child[1];
    }
    return pool;
}

void process_pool_wait_for_results(process_pool_t *pool, task_output_list_t *output_list) {
    while(wait(NULL) > 0); //等待所有子进程退出

    while (pool->done_count != pool->task_count) {
        for (int i = 0; i < pool->process_count; ++i) {
            task_output_t *output = calloc(1, sizeof(*output));
            if (read(pool->processes_pipes[i].read_pipe_fd, output, sizeof(*output)) == 0) {
                free(output);
                break;
            }
            list_push(output_list, output);
            pool->done_count += 1;
        }
    }
}

//将任务分发给进程池中的子进程，并等待子进程完成任务后收集结果
void process_pool_map(process_pool_t *pool, task_list_t *input_list, task_output_list_t *output_list) {
    pool->task_count = input_list->node_count;
    pool->done_count = 0;

    task_t *task = NULL;
    do {
        for (int i = 0; i < pool->process_count; ++i) {
            task = list_pop_back(input_list);
            if (!task)
                break;
            write(pool->processes_pipes[i].write_pipe_fd, task, sizeof(*task));
            printf("Sent task = %d\n", task->argument);
            free(task);
        }
    } while (task != NULL);

    for (int i = 0; i < pool->process_count; ++i) {
        close(pool->processes_pipes[i].write_pipe_fd);
    }

    process_pool_wait_for_results(pool, output_list);
}

void process_pool_free(process_pool_t *pool) {
    while(wait(NULL) > 0);
    for (int i = 0; i < pool->process_count; ++i) {
        close(pool->processes_pipes[i].read_pipe_fd);
    }
    free(pool->processes_pipes);
    free(pool);
}
