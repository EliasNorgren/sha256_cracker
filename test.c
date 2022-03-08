#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void check_empty();

int main()
{
  Queue *que = queue_create();

  queue_enqueue(que, "Hejsan");

  check_empty(que);

  char *s = queue_dequeue(que);
  printf("%s\n", s);

  check_empty(que);

  queue_destroy(que);
  free(s);

  return 0;
}

void check_empty(Queue *que)
{
  if (queue_is_empty(que)){
    printf("EMPTY\n");
  }
  else{
    printf("NOT EMPTY\n");
  }

}
