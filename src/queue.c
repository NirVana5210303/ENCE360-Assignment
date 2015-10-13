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
/*
Queue is a fixed buffer that is the size of the number of threads running
The buffer is in the form of a double linked list.
The buffer could possible be implemented as a cirular buffer
The get function pops the front of the queue, and returns that to a function.
The put function pushes a work item onto the queue.
*/
typedef struct NodeStruct
{
  void* next;
  void* prev;
  void* data;
} Node;

typedef struct QueueStruct
{
  int n_threads;
  int spaces;
  Node* head;
  Node* tail;
  pthread_mutex_t mutex;
  pthread_cond_t full;
  pthread_cond_t empty;
} Queue;

// Sets up a queue with enough spaces for one space for each thread
Queue *queue_alloc(int size)
{
  Queue* q = calloc(1, sizeof(Queue)); // allocate space in ram for queue
  q->n_threads = size;
  q->spaces = size; // queue starts empty
  q->head = NULL;
  q->tail = NULL;
  pthread_mutex_init(&q->mutex, NULL);
  pthread_cond_init(&q->full, NULL);
  pthread_cond_init(&q->empty, NULL);

  return q;
}

void queue_free(Queue *queue) {
  free(queue);
  return;
}

void queue_put(Queue *queue, void *item)
{
  pthread_mutex_lock(&queue->mutex);
  while (queue->spaces == 0)
  {
    pthread_cond_wait(&queue->full, &queue->mutex); // Wait if queue is full and can't push more into queue
  }
  Node *n = calloc(1, sizeof(Node)); // allocate space in ram for the Node
  n->data = item;
  n->next = NULL; //end of queue, nothing comes next.
  n->prev = queue->tail;
  if (queue->tail == NULL)
  {
    queue->tail = n;
    queue->head = n;
  }
  else
  {
    queue->tail->next = n;
    n->prev = queue->tail;
    queue->tail = n;
  }

  queue->spaces--;
  pthread_cond_signal(&queue->empty); // Signal to the get requests that there is now stuff on the queue
  pthread_mutex_unlock(&queue->mutex);
}

// pulls the variable at the head of the queue and returns it.
void *queue_get(Queue *queue)
{
  void *result = NULL;
  pthread_mutex_lock(&queue->mutex);
  while (queue->spaces == queue->n_threads) // If queue is empty, block and wait
  {
    pthread_cond_wait(&queue->empty, &queue->mutex);
  }

  Node *node = queue->head;
  if (node == queue->head && node == queue->tail)
  {
    queue->head = NULL;
    queue->tail = NULL;
  }
  else if (node == queue->head)
  {
    queue->head = node->next;
    queue->head->prev = NULL;
  }
  queue->spaces++;
  result = (void *)node->data;

  pthread_cond_signal(&queue->full);
  pthread_mutex_unlock(&queue->mutex);
  return result;
}
