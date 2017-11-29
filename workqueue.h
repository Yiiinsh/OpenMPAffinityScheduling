/* Threaded Programming Coursework 2 by B119172 */
/* Workqueue definition */
#ifndef __WORKQUEUE_H
#define __WORKQUEUE_H

#include <stdlib.h>

/*
 * Chunk for affinity scheduling 
 */
typedef struct _chunk
{
  int lo;       // lower bound
  int hi;       // higher bound
  int workload; // work load
} chunk;

/* 
 * Work queue node
 */
typedef struct _work_queue_node
{
  struct _work_queue_node *next; // pointer to next node
  struct _chunk chunk;           // work chunk
} work_queue_node;

/* 
 * Work queue
 */
typedef struct _work_queue
{
  struct _work_queue_node *front; // front of the queue
  struct _work_queue_node *rear;  // rear of the queue
  int chunk_size;                 // current chunk(node) size
  int workload;                   // current workload in the queue (total amount of iterations)
} work_queue;

/* 
 * Initialize a new empty work queue 
 * inout : queue, pointer to a work_queue
 */
void init_work_queue(work_queue *queue);

/* 
 * Destory a work queue
 * in : queue, pointer to the queue to be destoried
 */
void destroy_work_queue(work_queue *queue);

/* 
 * Enqueue a new chunk
 * in : queue, pointer to the queue
 * in : new_chunk, chunk to be added to the queue
 */
void enworkqueue(work_queue *queue, chunk new_chunk);

/* 
 * Dequeue from queue
 * in : queue, pointer to the queue
 * return : front element of the queue, return NULL if queue is empty
 */
chunk deworkqueue(work_queue *queue);

/* 
 * Print information about the work queue
 * in : queue, pointer to the queue
 */
void print_queue(work_queue *queue);

/* 
 * Check if the queue is empty
 * in : queue, pointer to the queue
 * return : flag represents if the queue is empty
 */
int is_queue_empty(work_queue *queue);

/* 
 * Get information about the most loaded thread
 * in : queue, pointer to the array of work queue
 * in : queue_cnt, totoal queue count in the work queue array
 * out : idx, index of the most loaded thread, -1 if all works are done
 * out : left_workload, left workload of the most loaded thread, 0 if all works are done
 */
void get_most_loaded(work_queue *queue, int queue_cnt, int *idx, int *left_workload);

#endif