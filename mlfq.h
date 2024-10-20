#ifndef MLFQ__H
#define MLFQ__H

#include <stdbool.h>

#include "timer.h"
#include "queue_.h"

#define MAX_QUEUE_SIZE 3

typedef struct MLFQ MLFQ;


struct MLFQ {
    Queue queues[MAX_QUEUE_SIZE];
    Queue register_queue;
};

void mlfq_scheduler(MLFQ *mlfq, Timer *timer);

void free_mlfq(MLFQ *mlfq);

void remove_(MLFQ *mlfq, Node_ *node);

bool register_(MLFQ *mlfq, Node_ *node);

bool insert_(MLFQ *mlfq, Node_ *node);

void test_bad_task();

void test_mlfq();

#define ALL_DOWN_(M) (((M)->register_queue).size) == 0


#endif //MLFQ__H
