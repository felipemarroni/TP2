#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

using namespace std;

#define QNTD_NUM 100000000
long sum;
int vec[QNTD_NUM];

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

//struct para passar mais de um argumento
struct arg_struct {
    int num_of_threads;
    int i;
};

//função que gera numeros aleatorios
void randVector(int vec[]){
    srand(time(NULL));
    for(unsigned long i = 0; i < QNTD_NUM; ++i){
        vec[i] = rand() % 201 + (-100);
    }
}

void* sum_of_numbers(void* arguments){
    struct arg_struct *args = (struct arg_struct *) arguments;
    int index_first_num = args->i;
    long uni_sum = 0;
    unsigned long size_of_segment = QNTD_NUM/(args->num_of_threads);
    unsigned long index = index_first_num * size_of_segment;
    unsigned long limit = (index_first_num + 1) * size_of_segment;

    //lidar com a situação em que N/K não é inteiro
    if (args->num_of_threads - 1 == args->i){
        limit += (QNTD_NUM % args->num_of_threads);
    }


    for (unsigned long i = index; i < limit; i++){
            uni_sum += vec[i];
    }

    pthread_mutex_lock(&mutex1);
    sum += uni_sum;
    pthread_mutex_unlock(&mutex1);

    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    struct arg_struct args;
    args.num_of_threads = atoll(argv[1]);
    clock_t t;
    t = clock();
    randVector(vec);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("%f \n", time_taken);

    //creating threads
    pthread_t thread_ids[args.num_of_threads];
    for (int i = 0; i < args.num_of_threads; i++){
        args.i = i;
        pthread_create(&thread_ids[i], NULL, sum_of_numbers, &args);
        //esperar pelas threads receberem os argumentos
        sleep(1);
    }

    //esperando as threads acabarem seu trabalho
    for (int i = 0; i < args.num_of_threads; i++){
        pthread_join(thread_ids[i], NULL);
    }
    printf("%ld \n", sum);

    return 0;
}
