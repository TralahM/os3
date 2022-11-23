/**
 * \file queue.h
 * Queue Implementation.
 */

#include <pthread.h>

/**
 * \struct QNode;
 * the Queue Node.
 */
struct QNode {
    char* data;
    struct QNode* next;
};

/**
 * \struct Queue;
 * the Queue.
 */
struct Queue {
    struct QNode *head, *tail;
    int size;
};

/**
 * Create new Qnode with this string.
 * @param data the string.
 */
struct QNode* newQNode(char* data);

/**
 * Create new Queue.
 */
struct Queue* createQueue();

/**
 * Create new Qnode with this string.
 * @param q the Queue.
 * @param str the string.
 */
void enQueue(struct Queue* q, char* str);

/**
 * DeQueue from this Queue.
 * @param q the Queue.
 */
char* deQueue(struct Queue* q);
