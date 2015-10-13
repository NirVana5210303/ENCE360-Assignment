
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

The buffer could possible be implemented as a cirular buffer and get the same results

The get function pops the front of the queue, and returns that to a function.

The put function pushes a work item onto the queue.

The queue is made up of nodes, which contain a pointer to the data. They are generic

In regards to navigation,
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
// No need for mutex as no threads are allocated queue yet
Queue *queue_alloc(int size)
{
  Queue* q = calloc(1, sizeof(Queue)); // allocate space in ram for queue
  //puts("allocated size");
  q->n_threads = size;
  q->spaces = size; // queue starts empty
  q->head = NULL;
  q->tail = NULL;
  //puts("set variables");
  // Initialising the mutex and signals
  pthread_mutex_init(&q->mutex, NULL);
  pthread_cond_init(&q->full, NULL);
  pthread_cond_init(&q->empty, NULL);
  //puts("set pthreads");

  return q;
}


// note needed for first test. Implement later
void queue_free(Queue *queue) {
  free(queue);
  return;
}

/*
 * queue_put:
 *
 * Place an item into the concurrent queue.
 *
 * If there is no space available then queue_put will
 * block until a space becomes available when it will
 * put the item into the queue and immediately return.
 *
 * Uses void* to hold an arbitrary type of item,
 * it is the users responsibility to manage memory
 * and ensure it is correctly typed.
 *
 */
void queue_put(Queue *queue, void *item)
{
  pthread_mutex_lock(&queue->mutex);
  while (queue->spaces == 0)
  {
    //puts("waiting for queue to empty");
    pthread_cond_wait(&queue->full, &queue->mutex); // Wait if queue is full and can't push more into queue
  }
  //puts("queue_put");
  //puts("Making node...");
  Node *n = calloc(1, sizeof(Node)); // allocate space in ram for the Node
  //puts("node ram set");
  n->data = item;
  //int *bar = (int *)item;
  //printf("foo: %d\n", *bar);
  n->next = NULL; //end of queue, nothing comes next.
  n->prev = queue->tail;
  //puts("n->prev = queue->tail;");
  if (queue->tail == NULL)
  {
    queue->tail = n;
    queue->head = n;
    //puts("queue->tail == NULL, head and tail set to n");
  }
  else
  {
    queue->tail->next = n;
    n->prev = queue->tail;
    queue->tail = n;
    //puts("queue->tail != NULL");
  }

  queue->spaces--;
  //printf("%d\n", queue->spaces);
  //queue->tail = n; // set pointer to tail to the new node added on
  pthread_cond_signal(&queue->empty); // Signal to the get requests that there is now stuff on the queue
  //puts("Queue not empty signal sent");
  pthread_mutex_unlock(&queue->mutex);
}

// Not sure why this is here?
void queue_put2(Queue *queue, void *item) {

  assert(0 && "not implemented yet!");

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
  if (queue->spaces == 16)
  {
    puts("not ideal");
  }
  //puts("queue_get");
  Node *node = queue->head;
  //puts("got node");
  //int *bar = (int *)node->data;
  //printf("get data: %d\n\n", *bar);
  if (node == NULL)
  {
    puts("Something fucked up here");
  }
  if (node == queue->head && node == queue->tail)
  {
    //printf("spaces: %d\n", queue->spaces);
    queue->head = NULL;
    queue->tail = NULL;
  }
  else if (node == queue->head)
  {
    queue->head = node->next;
    if (queue->head == NULL)
    {
      puts("Something went horribly wrong");
    }
    queue->head->prev = NULL;
  }
  queue->spaces++;
  result = (void *)node->data;

  pthread_cond_signal(&queue->full);
  pthread_mutex_unlock(&queue->mutex);
  //puts("returning node data");
  //int* xyz = (int*)result;
  //printf("xyz=%d\n", *xyz);
  return result;
  // Get pointer to head of queue, delete head of queue, signal to put method that there is space
  // Also queue->space++

  //assert(0 && "not implemented yet!");

}
