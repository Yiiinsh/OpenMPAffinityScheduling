#include <stdio.h>
#include <math.h>

#define N 729
#define reps 1000 
#include <omp.h> 

/*
 * Chunk for affinity scheduling 
 */
typedef struct _chunk {
  int lo; // lower bound
  int hi; // higher bound
  int workload; // work load
} chunk;

/* 
 * Work queue node
 */
typedef struct _work_queue_node {
  struct _work_queue_node *next; // pointer to next node
  struct _chunk chunk; // work chunk
} work_queue_node;

/* 
 * Work queue
 */
typedef struct _work_queue {
  struct _work_queue_node *front; // front of the queue
  struct _work_queue_node *rear; // rear of the queue
  int chunk_size; // current chunk(node) size
  int workload; // current workload in the queue (total amount of iterations)
} work_queue;

/* 
 * Initialize a new empty work queue 
 * inout : queue, pointer to a work_queue
 */
void init_work_queue(work_queue *queue) {
  if(NULL != queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->chunk_size = 0;
    queue->workload = 0; 
  } else {
    fprintf(stderr, "Cannot initialize a new work queue from NULL");
    exit(-1);
  }
}

/* 
 * Destory a work queue
 * in : queue, pointer to the queue to be destoried
 */
void destroy_work_queue(work_queue *queue) {
  if(NULL != queue) {
    while(NULL != queue->front) {
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
void enqueue(work_queue *queue, chunk new_chunk) {
  if(NULL != queue) {
    work_queue_node *new_node = (work_queue_node *) malloc(sizeof(work_queue_node));
    new_node->next = NULL;
    new_node->chunk.lo = new_chunk.lo;
    new_node->chunk.hi = new_chunk.hi;
    new_node->chunk.workload = new_chunk.workload;

    if(NULL == queue->front) {
      queue->front = new_node;
      queue->rear = new_node;
    } else {
      queue->rear->next = new_node;
      queue->rear = queue->rear->next;
    }
    queue->workload += new_node->chunk.workload;
    ++queue->chunk_size;
  } else {
    fprintf(stderr, "Cannot enqueue to a NULL queue");
  }
}

/* 
 * Dequeue from queue
 * in : queue, pointer to the queue
 * return : front element of the queue, return NULL if queue is empty
 */
chunk dequeue(work_queue *queue) {
  chunk new_chunk;
  new_chunk.lo = 0;
  new_chunk.hi = 0;
  new_chunk.workload = 0;

  if(NULL != queue) {
    if(NULL != queue->front) {
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
void print_queue(work_queue *queue) {
  if(NULL != queue) {
    work_queue_node *node = queue->front;
    while(NULL != node) {
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
int is_queue_empty(work_queue *queue) {
  if(NULL != queue) {
    return (queue->chunk_size == 0) ? 1 : 0;
  }
}

int get_most_workload_idx(work_queue *queue, int queue_cnt) {
  int most_workload_idx = -1;
  int most_work_load = 0;
  for(int i = 0; i < queue_cnt; ++i) {
    if(queue[i].workload > most_work_load) {
      most_work_load = queue[i].workload;
      most_workload_idx = i;
    }
  }
  return most_workload_idx;
}

double a[N][N], b[N][N], c[N];
int jmax[N];  

void init1(void);
void init2(void);
void runloop(int); 
void loop1chunk(int, int);
void loop2chunk(int, int);
void valid1(void);
void valid2(void);

int main(int argc, char *argv[]) { 

  double start1,start2,end1,end2;
  int r;

  init1(); 

  start1 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(1);
  } 

  end1  = omp_get_wtime();  

  valid1(); 

  printf("Total time for %d reps of loop 1 = %f\n",reps, (float)(end1-start1)); 


  init2(); 

  start2 = omp_get_wtime(); 

  for (r=0; r<reps; r++){ 
    runloop(2);
  } 

  end2  = omp_get_wtime(); 

  valid2(); 

  printf("Total time for %d reps of loop 2 = %f\n",reps, (float)(end2-start2)); 

} 

void init1(void){
  int i,j; 

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      a[i][j] = 0.0; 
      b[i][j] = 3.142*(i+j); 
    }
  }

}

void init2(void){ 
  int i,j, expr; 

  for (i=0; i<N; i++){ 
    expr =  i%( 3*(i/30) + 1); 
    if ( expr == 0) { 
      jmax[i] = N;
    }
    else {
      jmax[i] = 1; 
    }
    c[i] = 0.0;
  }

  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      b[i][j] = (double) (i*j+1) / (double) (N*N); 
    }
  }
 
} 

void runloop(int loopid)  {
  work_queue *work_queues;
  omp_lock_t *work_queue_locks;
#pragma omp parallel default(none) shared(loopid, work_queues, work_queue_locks)
  {
    int myid  = omp_get_thread_num();
    int nthreads = omp_get_num_threads(); 

    /* Work breakdown */
#pragma omp single
    {
      /* Init work queues and locks */
      work_queues = (work_queue *) malloc(nthreads * sizeof(work_queue));
      work_queue_locks = (omp_lock_t *) malloc(nthreads * sizeof(omp_lock_t));
      for(int i = 0; i < nthreads; ++i) {
        init_work_queue(&work_queues[i]);
        omp_init_lock(&work_queue_locks[i]);
      }
    }

    /* Local work set break down */
    int ipt = (int) ceil((double) N / (double) nthreads);
    int local_work_set_start = myid * ipt;
    int local_work_set_end = (myid + 1) * ipt;
    local_work_set_end = (local_work_set_end > N) ? N : local_work_set_end;
    
    while(local_work_set_start < local_work_set_end) {
      int workload = (int) ceil((double)(local_work_set_end - local_work_set_start) / (double)nthreads);
      chunk new_chunk;
      new_chunk.lo = local_work_set_start;
      new_chunk.hi = local_work_set_start + workload;
      new_chunk.workload = workload;

      /* Fill in thread corresponding work queue */
      enqueue(&work_queues[myid], new_chunk);

      local_work_set_start = local_work_set_start + workload;
    }
#pragma omp barrier

    /* Local work execution */
    while(! is_queue_empty(&work_queues[myid])) {
      chunk current_work;
      omp_set_lock(&work_queue_locks[myid]);
      current_work = dequeue(&work_queues[myid]);
      omp_unset_lock(&work_queue_locks[myid]);

      switch(loopid) {
        case 1: loop1chunk(current_work.lo,current_work.hi); break;
        case 2: loop2chunk(current_work.lo,current_work.hi); break;
      }
    }

    /* Work stealing */
    int idx;
    while(-1 != (idx = get_most_workload_idx(work_queues, nthreads))) {
      chunk stealed_work;
      omp_set_lock(&work_queue_locks[idx]);
      stealed_work = dequeue(&work_queues[idx]);
      omp_unset_lock(&work_queue_locks[idx]);

      switch(loopid) {
        case 1: loop1chunk(stealed_work.lo,stealed_work.hi); break;
        case 2: loop2chunk(stealed_work.lo,stealed_work.hi); break;
      }
    }
#pragma omp barrier

    /* Resource release */
    destroy_work_queue(&work_queues[myid]);
    omp_destroy_lock(&work_queue_locks[myid]);
  }

  if(NULL != work_queues) {
    free(work_queues);
  }
  if(NULL != work_queue_locks) {
    free(work_queue_locks);
  }
}

void loop1chunk(int lo, int hi) { 
  int i,j; 
  
  for (i=lo; i<hi; i++){ 
    for (j=N-1; j>i; j--){
      a[i][j] += cos(b[i][j]);
    } 
  }

} 

void loop2chunk(int lo, int hi) {
  int i,j,k; 
  double rN2; 

  rN2 = 1.0 / (double) (N*N);  

  for (i=lo; i<hi; i++){ 
    for (j=0; j < jmax[i]; j++){
      for (k=0; k<j; k++){ 
	c[i] += (k+1) * log (b[i][j]) * rN2;
      } 
    }
  }

}

void valid1(void) { 
  int i,j; 
  double suma; 
  
  suma= 0.0; 
  for (i=0; i<N; i++){ 
    for (j=0; j<N; j++){ 
      suma += a[i][j];
    }
  }
  printf("Loop 1 check: Sum of a is %lf\n", suma);

} 

void valid2(void) { 
  int i; 
  double sumc; 
  
  sumc= 0.0; 
  for (i=0; i<N; i++){ 
    sumc += c[i];
  }
  printf("Loop 2 check: Sum of c is %f\n", sumc);
} 