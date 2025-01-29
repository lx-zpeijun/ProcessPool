#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

#include "process_pool.h"
#include "list.h"

task_output_t check_prime(void *dest) {
    task_output_t output = {.number = dest, .is_prime = 0};

    if (output.number == 1 || output.number == 0)
        return output;

    for (int i = 2; i < output.number ; ++i) {
        if (output.number % i == 0)
            return output;
    }
    output.is_prime = 1;
    return output;
}


int check_number_of_primes(int from, int to) {
    int number_of_processes = get_nprocs() - 1;
    int count = to - from + 1;

    process_pool_t *pool = process_pool_init(number_of_processes);
    task_list_t *input_list = calloc(1, sizeof(task_list_t));
    task_output_list_t *output_list = calloc(1, sizeof(task_output_list_t));

    for (int i = 0; i < count; ++i) {
        task_t *task = calloc(1, sizeof(task_t));
        task->function = check_prime;
        task->argument = i + from;
        list_push(input_list, task);
    }

    process_pool_map(pool, input_list, output_list);

    int number_of_primes = 0;
    for (int i = 0; i < count; ++i) {
        task_output_t *output = list_pop_back(output_list);
        number_of_primes += output->is_prime;
        printf("Number: %d is prime: %d\n", output->number, output->is_prime);
        free(output);
    }

    process_pool_free(pool);
    list_free(input_list);
    list_free(output_list);

    return number_of_primes;
}

int main() {
    int from = 1;
    int to = 101;
    int n = check_number_of_primes(from, to);
    printf("Number of primes between %d and %d is %d", from, to, n);
    return 0;
}
