#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <vector>


sem_t mutex;
sem_t empty1;
sem_t full;
std::vector<int> buffer;
long contador = 0;


int produzir(){  //produzir um elemento
	int item;
    item = (rand()%10000000) + 1;
	return item;
}


void inserir(int produto, int N){  //inserir produto se encontrar um 0
    for (int i = 0; i < N; i++){
        if (buffer[i] == 0){
            buffer[i] = produto;
            return;
        }
	}
	return;
}


int remover(int N){
	int produto = 0;
    for (int i = 0; i < N; i++){
        if (buffer[i] == 0){  //se encontrar 0 na frente, remove o elemento anterior
            produto = buffer[i - 1];
            buffer[i - 1] = 0;
            return produto;
        }
        if (i == N - 1){  //chegar no final de um buffer cheio
            produto = buffer[i];
            buffer[i] = 0;
            return produto;
        }
    }
    return 0;
}

void primo(int n) {
   int i, flag = 0;
   for(i=2; i<=n/2; ++i) {
      if(n%i==0) {
         flag=1;
         break;
      }
   }
   if (flag==0){
      //printf("N = %d e primo\n", n);
    } else {
      //printf("N = %d nao e primo\n", n);
    }
}

void *produtor(void* arg){
	int *N_ptr = (int *) arg;
	int N = *N_ptr;
	long produto;
	while (true){
		produto = produzir();
		sem_wait(&empty1);
		sem_wait(&mutex);
		inserir(produto, N);
		sem_post(&mutex);
		sem_post(&full);
	}
}

void *consumidor(void* arg){
	int *N_ptr = (int *) arg;
	int N = *N_ptr;
	long produto;
	while (true){
        sem_wait(&full);
        sem_wait(&mutex);
        produto = remover(N);
        contador++;
        sem_post(&mutex);
        sem_post(&empty1);
        primo(produto);
        if (contador == 100000){
            exit(0);
        }
	}
}

/********* funcao principal***********/
int main (int argc, char**argv){

    srand(time(NULL));
	int Np = atoi(argv[1]);
	int Nc = atoi(argv[2]);
	int N = atoi(argv[3]);
	for (int i = 0; i < N; i++){
        buffer.push_back(0);
	}

	sem_init(&mutex,0,1);
	sem_init(&empty1,0,N);
	sem_init(&full,0,0);

	pthread_t cons[Nc];
	pthread_t prod[Np];

 	int i=0;
	int j=0;
	for (i; i<Nc; i++){
		pthread_create(&cons[i], NULL, consumidor, &N);
    }
	for(j; j<Np; j++){
		pthread_create(&prod[j], NULL, produtor, &N);
	}
	for(i=0; i<Nc; i++){
		pthread_join(cons[i],0);
	}
	for(j=0; j<Np; j+=1){
		pthread_join(prod[j],0);
	}

	return 0;

}
