//
// Created by wenshen on 24-10-9.
//

#ifndef QUEUE__H
#define QUEUE__H


typedef struct Node_ Node_;
typedef struct Queue_ Queue;


struct Queue_ {
    Node_ *front;
    Node_ *tail;
    int size;
};

struct Node_ {
    Node_ *prev;
    void *data;
    Node_ *next;
};


void free_node(Node_ *node);

void init_queue(Queue *queue);

void remove_node(Queue *queue, Node_ *node);

void free_queue(Queue *queue);

void enqueue(Queue *queue, Node_ *node);

Node_ *dequeue(Queue *queue);

Node_ *create_node(void *data);


#define QUEUE_EMPTY(queue) ((queue)->size == 0)

#endif //QUEUE__H
