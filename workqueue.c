/* Threaded Programming Coursework 2 by B119172 */
/* Workqueue implementation */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "workqueue.h"

/* 
 * Initialize a new empty work queue 
 * inout : queue, pointer to a work_queue
 */
void init_work_queue(work_queue *queue)
{
  if (NULL != queue)
  {
    queue->front = NULL;
    queue->rear = NULL;
    queue->chunk_size = 0;
    queue->workload = 0;
  }
  else
  {
    fprintf(stderr, "Cannot initialize a new work queue from NULL");
    exit(-1);
  }
}

/* 
 * Destory a work queue
 * in : queue, pointer to the queue to be destoried
 */
void destroy_work_queue(work_queue *queue)
{
  if (NULL != queue)
  {
    while (NULL != queue->front)
    {
      work_queue_node *tmp;
      tmp = queue->front;
      queue->front = queue->front->next;
      free(tmp);
    }
    queue->rear = queue->front;
    queue->chunk_size = 0;
    queue->workload = 0;
  }
}

/* 
 * Enqueue a new chunk
 * in : queue, pointer to the queue
 * in : new_chunk, chunk to be added to the queue
 */
void enworkqueue(work_queue *queue, chunk new_chunk)
{
  if (NULL != queue)
  {
    work_queue_node *new_node = (work_queue_node *)malloc(sizeof(work_queue_node));
    new_node->next = NULL;
    new_node->chunk.lo = new_chunk.lo;
    new_node->chunk.hi = new_chunk.hi;
    new_node->chunk.workload = new_chunk.workload;

    if (NULL == queue->front)
    {
      queue->front = new_node;
      queue->rear = new_node;
    }
    else
    {
      queue->rear->next = new_node;
      queue->rear = queue->rear->next;
    }
    queue->workload += new_node->chunk.workload;
    ++queue->chunk_size;
  }
  else
  {
    fprintf(stderr, "Cannot enqueue to a NULL queue");
  }
}

/* 
 * Dequeue from queue
 * in : queue, pointer to the queue
 * return : front element of the queue, return NULL if queue is empty
 */
chunk deworkqueue(work_queue *queue)
{
  chunk new_chunk;
  new_chunk.lo = 0;
  new_chunk.hi = 0;
  new_chunk.workload = 0;

  if (NULL != queue)
  {
    if (NULL != queue->front)
    {
      work_queue_node *tmp_node = queue->front;
      queue->front = queue->front->next;
      queue->workload -= tmp_node->chunk.workload;
      --queue->chunk_size;

      new_chunk.lo = tmp_node->chunk.lo;
      new_chunk.hi = tmp_node->chunk.hi;
      new_chunk.workload = tmp_node->chunk.workload;

      free(tmp_node);
    }
  }
  return new_chunk;
}

/* 
 * Print information about the work queue
 * in : queue, pointer to the queue
 */
void print_queue(work_queue *queue)
{
  if (NULL != queue)
  {
    work_queue_node *node = queue->front;
    while (NULL != node)
    {
      printf("[%d,%d) ", node->chunk.lo, node->chunk.hi);
      node = node->next;
    }
    printf("\n");
  }
}

/* 
 * Check if the queue is empty
 * in : queue, pointer to the queue
 * return : flag represents if the queue is empty
 */
int is_queue_empty(work_queue *queue)
{
  if (NULL != queue)
  {
    return (queue->chunk_size == 0) ? 1 : 0;
  }
}

/* 
 * Get information about the most loaded thread
 * in : queue, pointer to the array of work queue
 * in : queue_cnt, totoal queue count in the work queue array
 * out : idx, index of the most loaded thread, -1 if all works are done
 * out : left_workload, left workload of the most loaded thread, 0 if all works are done
 */
void get_most_loaded(work_queue *queue, int queue_cnt, int *idx, int *left_workload)
{
  *idx = -1;
  *left_workload = 0;
  for (int i = 0; i < queue_cnt; ++i)
  {
    if (queue[i].workload > *left_workload)
    {
      *left_workload = queue[i].workload;
      *idx = i;
    }
  }
}