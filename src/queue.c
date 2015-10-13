
#include "queue.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct QueueStruct
{
  int n_threads;
  int spaces;
  void* head;
  void* tail;
  pthread_mutex_t mutex;
  pthread_cond_t full;
  pthread_cond_t empty;
} Queue;

typedef struct NodeStruct
{
  void* next;
  void* prev;
  void* data;
} Node;


Queue *queue_alloc(int size)
{
  Queue* q = malloc(sizeof(Queue));
  q->n_threads = size;
  q->spaces = size;
  pthread_mutex_init(&q->mutex, NULL);
  //q->mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_init(&q->full, NULL);
  pthread_cond_init(&q->empty, NULL);
  //q->full = PTHREAD_COND_INITIALIZER;
  //q->empty = PTHREAD_COND_INITIALIZER;

  return q;
}

void queue_free(Queue *queue) {
  // Frees up entire queue
  assert(0 && "not implemented yet!");
}

void queue_put(Queue *queue, void *item) {
  pthread_mutex_lock(&queue->mutex);
  while (!queue->spaces)
  {
    pthread_cond_wait(&queue->full, &queue->mutex);
  }
  Node *n = (Node*)malloc(sizeof(Node));
  n->data = item;
  n->next = queue->tail;
  n->prev = NULL;

  queue->spaces--;
  queue->tail = n;
  pthread_cond_signal(&queue->empty);
  pthread_mutex_unlock(&queue->mutex);
}

void queue_put2(Queue *queue, void *item) {

  assert(0 && "not implemented yet!");

}



void *queue_get(Queue *queue) {
  pthread_mutex_lock(&queue->mutex);
  if (queue->spaces == queue->n_threads)
  {
    pthread_cond_wait(&queue->empty, &queue->mutex);
  }


  queue->spaces++;
  pthread_cond_signal(&queue->full);
  pthread_mutex_unlock(&queue->mutex);

  // Get pointer to head of queue, delete head of queue, signal to put method that there is space
  // Also queue->space++

  assert(0 && "not implemented yet!");

}
