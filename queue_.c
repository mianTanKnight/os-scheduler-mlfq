//
// Created by wenshen on 24-10-9.
//

#include "queue_.h"

#include <stdio.h>
#include "task_.h"
#include "stdlib.h"

void init_queue(Queue *queue) {
    if (!queue)
        return;
    queue->front = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

void remove_node(Queue *queue, Node_ *node) {
    if (QUEUE_EMPTY(queue) || !node)
        return;

    Node_ *prev = node->prev;
    Node_ *next = node->next;

    if (prev)
        prev->next = node->next;
    if (next)
        next->prev = prev;
    if (!prev)
        queue->front = next;
    if (!next)
        queue->tail = prev;

    node->prev = NULL;
    node->next = NULL;
    queue->size--;
}

Node_ *create_node(void *data) {
    Node_ *node = (Node_ *) malloc(sizeof(Node_));
    if (!node) {
        printf("Failed to allocate memory for node.\n");
        return NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    node->data = data;
    return node;
}

void free_node(Node_ *node) {
    if (!node)
        return;
    destroy_(node->data);
    free(node);
}

void free_queue(Queue *queue) {
    if (!queue)
        return;
    Node_ *current = queue->front;
    while (current) {
        Node_ *next = current->next;
        destroy_(next->data);
        free(current);
        current = next;
    }
    queue->front = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

void enqueue(Queue *queue, Node_ *node) {
    if (queue->size == 0) {
        queue->front = queue->tail = node;
    } else {
        queue->tail->next = node;
        node->prev = queue->tail;
        queue->tail = node;
    }
    queue->front->prev = NULL;
    queue->tail->next = NULL;
    queue->size++;
}

Node_ *dequeue(Queue *queue) {
    if (!queue || queue->size == 0)
        return NULL;
    Node_ *node = queue->front;
    if (queue->size == 1) {
        queue->front = queue->tail = NULL;
    } else {
        node->next->prev = NULL;
        queue->front = node->next;
    }
    queue->size--;
    if (queue->size == 0) {
        queue->front = queue->tail = NULL;
    }
    return node;
}
