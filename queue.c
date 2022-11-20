/*
 * =====================================================================================
 *
 *       Filename:  queue.c
 *
 * =====================================================================================
 */

#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

struct QNode* newQNode(char* data) {
    struct QNode* tmp = (struct QNode*)malloc(sizeof(struct QNode));
    tmp->data = data;
    tmp->next = NULL;
    return tmp;
}

struct Queue* createQueue() {
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}

void enQueue(struct Queue* q, char* data) {
    struct QNode* tmp = newQNode(data);
    if (q->tail == NULL) {
        q->head = q->tail = tmp;
        q->size += 1;
        return;
    }
    q->tail->next = tmp;
    q->tail = tmp;
    q->size += 1;
}

char* deQueue(struct Queue* q) {
    if (q->head == NULL) {
        return NULL;
    }
    struct QNode* tmp = q->head;
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    q->size -= 1;
    return tmp->data;
}
