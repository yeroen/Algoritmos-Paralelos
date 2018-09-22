#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct list_node_s{
  int data;
  struct list_node_s* next;
};

int Member(int value, struct list_node_s* head_p){
  struct list_node_s* curr_p = head_p;

  while(curr_p != NULL && curr_p->data < value){
    curr_p = curr_p->next;
  }

  if(curr_p == NULL || curr_p->data > value){
    return 0;
  }
  else {
    return 1;
  }
}

int Insert(int value, struct list_node_s** head_p){
  struct list_node_s* curr_p = *head_p;
  struct list_node_s* pred_p = NULL;
  struct list_node_s* temp_p;

  while(curr_p != NULL && curr_p->data < value){
    pred_p = curr_p;
    curr_p = curr_p->next;
  }

  if(curr_p == NULL || curr_p->data > value){
    temp_p = malloc(sizeof(struct list_node_s));
    temp_p->data = value;
    temp_p->next = curr_p;
    if(pred_p == NULL)
      *head_p = temp_p;
    else pred_p->next = temp_p;
    return 1;
  }
  else return 0;
}

int Delete(int value, struct list_node_s** head_p){
  struct list_node_s* curr_p = *head_p;
  struct list_node_s* pred_p = NULL;

  while(curr_p != NULL && curr_p->data < value){
    pred_p = curr_p;
    curr_p = curr_p->next;
  }

  if(curr_p != NULL && curr_p->data == value){
    if(pred_p == NULL){
      *head_p = curr_p->next;
      free(curr_p);
    }
    else{
      pred_p->next = curr_p->next;
      free(curr_p);
    }
    return 1;
  }
  else{
    return 0;
  }
}

struct list_node_s* list = NULL;

void N_Insert(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_mutex_lock(&mutex);
    Insert(1000,&list);
    pthread_mutex_unlock(&mutex);
  }
  return;
}

void N_Member(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_mutex_lock(&mutex);
    Member(1000,list);
    pthread_mutex_unlock(&mutex);
  }
  return;
}

void N_Delete(double ops){
  double i;
  for(i = 0; i < ops; i++) {
    pthread_mutex_lock(&mutex);
    Delete(1000,&list);
    pthread_mutex_unlock(&mutex);
  }
  return;

}

double members, inserts, deletes;

void* N_All(void* rank){
  long my_rank = (long) rank;
  N_Member(members);
  N_Insert(inserts);
  N_Delete(deletes);
  return NULL;
}

int main(int argc, char* argv[]){
  long thread;
  double thread_count,i, elapsed;
  pthread_t *thread_handles;
  struct list_node_s* list = NULL;
  struct timespec begin,end;

  srand(4);
  thread_count = strtol(argv[1],NULL,10);
  thread_handles = malloc(thread_count* sizeof(pthread_t));

  scanf("%lf %lf %lf", &members, &inserts, &deletes);

  members = (members * 1000.0)/thread_count;
  inserts = (inserts * 1000.0)/thread_count;
  deletes = (deletes * 1000.0)/thread_count;

  for(i = 0; i < 1000; i++){
    Insert(i,&list);
  }

  clock_gettime(CLOCK_MONOTONIC, &begin);
  
  for(thread = 0; thread < thread_count; thread++)
    pthread_create(&thread_handles[thread], NULL, N_All, (void*) thread);
  for(thread = 0; thread < thread_count; thread++)
    pthread_join(thread_handles[thread], NULL);

  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

  printf("%lf\n", elapsed);
  return 0;
}
