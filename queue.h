/**
 * @defgroup Queue
 *
 * @brief The module is an Queue.
 *
 * (Detailed description of the module.)
 *
 * This is a Queue implemented with a doubly linked list.
 *
 * It contains different functions to operate the Queue.
 *
 *
 *
 * @author  Elias Norgren
 * @since   2019-11-26
 *
 * @{
 */


#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#include "list.h"

/**
 * @brief The type for the queue.
 *
 * Contains a pointer to a List.
 *
 */
typedef struct queue
{
    List *list;
    int size;
} Queue;


/**
 * @brief Create and return an empty queue.
 *
 * Allocates memory for a Queue and creates a List.
 *
 * It is the callers responsibility to free the Queue and its elements using
 * queue_destroy().
 *
 * @return           Pointer to Queue
 */
Queue *queue_create(void);

/**
 * @brief Destroy the queue.
 *
 * Destroys the List then frees the Queue.
 *
 *
 * @param q      pointer to Queue
 * @return           -
 */
void queue_destroy(Queue *q);

/**
 * @brief Add a value to the tail of the queue.
 *
 * The value is copied to dynamically allocated memory.
 * To remove al elements queue_destroy() can be used.
 *
 * @param q          pointer to Queue
 * @param value      String to enqueue.
 * @return           -
 */
void queue_enqueue(Queue *q, const char *value);

/**
 * @brief Remove the value at the head of the queue.
 *
 * The caller is responsible for deallocating the returned pointer.
 *
 * @param q      pointer to Queue
 * @return       String that was on the removed element.
 */
char *queue_dequeue(Queue *q);

/**
 * @brief Check if the queue is empty.
 *
 * Looks if the Queue is empty or not.
 *
 *
 *
 * @param q      pointer to Queue
 * @return       True if empty. Else false. 
 */
bool queue_is_empty(const Queue *q);

int queue_size(const Queue *q);

#endif /* QUEUE_H */
