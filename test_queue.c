/*
 * =====================================================================================
 *
 *       Filename:  test_queue.c
 *
 *    Description: Test Queue
 *
 *        Version:  1.0
 *
 *        Created:  11/18/2022 10:04:10 PM
 *
 *       Revision:  none
 *
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#include <stdio.h>

#include "queue.h"
int main(int argc, char* argv[]) {
    struct Queue* q = createQueue();
    enQueue(q, "one");
    enQueue(q, "two");
    enQueue(q, "three");
    enQueue(q, "four");
    enQueue(q, "five");
    enQueue(q, "six");
    enQueue(q, "seven");
    for (int i = 0; i < 8; i++) {
        printf("%d deQueue: %s\n", i, deQueue(q));
    }
    return 0;
}
