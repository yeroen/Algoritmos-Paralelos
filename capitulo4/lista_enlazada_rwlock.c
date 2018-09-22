#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_rwlock_t rwlock;

struct nodo{
  int dato;
  struct nodo* sig;
};

int buscar(int valor, struct nodo* head_p){
  struct nodo* p = head_p;
  while(p != NULL && p->dato < valor)
    p = p->sig;

  if(p == NULL || p->dato > valor) return 0;
  else return 1;
}

int insertar(int valor, struct nodo** head_p){
  struct nodo* p = *head_p;
  struct nodo* pred_p = NULL;
  struct nodo* temp_p;

  while(p != NULL && p->dato < valor){
    pred_p = p;
    p = p->sig;
  }

  if(p == NULL || p->dato > valor){
    temp_p = malloc(sizeof(struct nodo));
    temp_p->dato = valor;
    temp_p->sig = p;
    if(pred_p == NULL)
      *head_p = temp_p;
    else pred_p->sig = temp_p;
    return 1;
  }
  else return 0;
}

int borrar(int valor, struct nodo** head_p){
  struct nodo* p = *head_p;
  struct nodo* pred_p = NULL;

  while(p != NULL && p->dato < valor){
    pred_p = p;
    p = p->sig;
  }

  if(p != NULL && p->dato == valor){
    if(pred_p == NULL){
      *head_p = p->sig;
      free(p);
    }
    else{
      pred_p->sig = p->sig;
      free(p);
    }
    return 1;
  }
  else{
    return 0;
  }
}

struct nodo* list = NULL;

void N_insertar(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_rwlock_wrlock(&rwlock);
    insertar(1000,&list);
    pthread_rwlock_unlock(&rwlock);
  }
  return;
}

void N_buscar(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_rwlock_rdlock(&rwlock);
    buscar(1000,list);
    pthread_rwlock_unlock(&rwlock);
  }
  return;
}

void N_borrar(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_rwlock_wrlock(&rwlock);
    borrar(1000,&list);
    pthread_rwlock_unlock(&rwlock);
  }
  return;

}

double buscados, insertados, borrados;

void* N_todo(void* rank){
  long my_rank = (long) rank;
  N_buscar(buscados);
  N_insertar(insertados);
  N_borrar(borrados);
  return NULL;
}


int main(int argc, char* argv[]){
  long thread;
  double thread_count,i, elapsed;
  pthread_t *thread_handles;
  struct nodo* list = NULL;
  struct timespec begin,end;

  pthread_rwlock_init(&rwlock,NULL);

  thread_count = strtol(argv[1],NULL,10);
  thread_handles = malloc(thread_count* sizeof(pthread_t));

  scanf("%lf %lf %lf", &buscados, &insertados, &borrados);

  buscados = (buscados * 1000.0)/thread_count;
  insertados = (insertados * 1000.0)/thread_count;
  borrados = (borrados * 1000.0)/thread_count;

  for(i = 0; i < 1000; i++){
    insertar(i,&list);
  }

  clock_gettime(CLOCK_MONOTONIC, &begin);

  for(thread = 0; thread < thread_count; thread++)
    pthread_create(&thread_handles[thread], NULL, N_todo, (void*) thread);
  for(thread = 0; thread < thread_count; thread++)
    pthread_join(thread_handles[thread], NULL);

  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

  printf("%lf\n", elapsed);
  return 0;
}
