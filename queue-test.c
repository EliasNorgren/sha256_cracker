#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void add_values(Queue *que);
bool verify_values(Queue *que);


int main()
{
  // Creates Queue.
  Queue *que = queue_create();

  // Add values A-Z
  add_values(que);

  bool ok = false;
  //Checks that values are correct.
  ok = verify_values(que);
  printf("Test the functioning of the queue ... %s\n", ok? "PASS" : "FAIL");

  // Destroys Queue.
  queue_destroy(que);

  return 0;
}

void add_values(Queue *que)
{
  char string[2] = "A";

  for (char c = 'A'; c <= 'Z'; c++) {

      string[0] = c;
      queue_enqueue(que, string);
  }
}

bool verify_values(Queue *que)
{
  for (char ch = 'A'; ch <= 'Z'; ch++) {

      char *str = queue_dequeue(que);

      if ( *str != ch){

        free(str);
        return false;
      }

      free(str);
  }

  return true;
}
