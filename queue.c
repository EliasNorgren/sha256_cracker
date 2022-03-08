#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

// Copy string (internal function)
// The caller is responsible for freeing the returned pointer.
static char *clone_string(const char *in)
{
    size_t len = strlen(in);
    char *out = calloc(len + 1, sizeof(char));
    strcpy(out, in);
    return out;
}

Queue *queue_create(void)
{
  Queue *que = malloc(sizeof(Queue));
  que -> list = list_create();

  return que;
}

void queue_destroy(Queue *q)
{

  list_destroy(q -> list);
  free (q);
}

// Enqueues values from the end of the list.
void queue_enqueue(Queue *q, const unsigned char *value)
{
  list_insert(list_end(q -> list) , value);
}

// Dequeues values from the first list position.
unsigned char *queue_dequeue(Queue *q)
{

  ListPos first = list_first(q -> list);

  char *str = clone_string(first.node -> value);

  list_remove(first);

  return str;
}

bool queue_is_empty(const Queue *q)
{
  return list_is_empty(q -> list);
}
