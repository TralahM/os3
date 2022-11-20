/*
 *
 * =====================================================================================
 */

#include <pthread.h>

struct QNode {
    char* data;
    struct QNode* next;
};

struct Queue {
    struct QNode *head, *tail;
    int size;
};

struct QNode* newQNode(char*);

struct Queue* createQueue();

void enQueue(struct Queue*, char*);

char* deQueue(struct Queue*);
