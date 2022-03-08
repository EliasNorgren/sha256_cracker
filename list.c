#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"


/* ---------------------- Internal functions ---------------------- */

/*
 * @brief Clones a string and allocates memory for it.
 *
 * Takes a string as input and allocate it onto another string.
 * The strings node has to be removed for the allocated string to deallocate.
 *
 * @param in             A string.
 * @return               Same string as input but dynamically allocated
 */
static unsigned char *clone_string(const unsigned char *in)
{
    size_t len = strlen((char*) in);
    unsigned char *out = calloc(len + 1, sizeof(unsigned char));
    strcpy((char*) out, (char*) in);
    return out;
}

/*
 * @brief Allocates a new node and returns it with input value.
 *
 * Allocates memory for a node. Changes its value with the copy_string()
 * function. Sets its next/prev pointers to NULL.
 *
 * It is the callers responsibility to deallocate the node with list_remove().
 *
 * @param value       A string.
 * @return                -
 */
static struct node *make_node(const unsigned char *value)
{

   struct node *new_node = malloc(sizeof(struct node));
   new_node -> value = clone_string(value);
   new_node -> prev = NULL;
   new_node -> next = NULL;

   return new_node;

}

/* ---------------------- External functions ---------------------- */

List *list_create(void)
{

   List *new_list = malloc(sizeof(List));
   new_list -> head.next = &(new_list -> head);
   new_list -> head.prev = &(new_list -> head);
   new_list -> head.value = NULL;

   return new_list;
}


void list_destroy(List *lst)
{
   if ( ! list_is_empty(lst)){

      ListPos first = list_first(lst);
      ListPos end = list_end(lst);

      while (! list_pos_equal(first,end)) {
         ListPos first_next = list_next(first);
         list_remove(first);
         first = first_next;
      }
   }

   free(lst);

}

bool list_is_empty(const List *lst)
{

   if (lst -> head.next == &lst -> head){
      return true;
   }
   else{
      return false;
   }
}


ListPos list_first(List *lst)
{
    ListPos pos = {
        .node = lst->head.next
    };
    return pos;
}


ListPos list_end(List *lst)
{

    ListPos pos = {
        .node = &lst->head
    };
    return pos;

}


bool list_pos_equal(ListPos p1, ListPos p2)
{

   if (p1.node == p2.node){
      return true;
   }
   else{
      return false;
   }
}


ListPos list_next(ListPos pos)
{
   ListPos next = { .node = pos.node -> next};
   return next;
}


ListPos list_prev(ListPos pos)
{
   ListPos prev = { .node = pos.node -> prev};
   return prev;
}


ListPos list_insert(ListPos pos, const unsigned char *value)
{
    // Create a new node.
    struct node *node = make_node(value);

    // Find nodes before and after (may be the same node: the head of the list).
    struct node *before = pos.node->prev;
    struct node *after = pos.node;

    // Link to node after.
    node->next = after;
    after->prev = node;

    // Link to node before.
    node->prev = before;
    before->next = node;

    // Return the position of the new element.
    pos.node = node;
    return pos;
}


ListPos list_remove(ListPos pos)
{

   //Creates two node pointers that are inbetween the element that will be
   //removed.
   struct node *before = pos.node -> prev;
   struct node *after = pos.node -> next;

   //Connects these two pointers. The pos.node is now disconnected.
   before -> next = after;
   after -> prev = before;

   //Deallocates the memory in the pos.node and the value in the node.
   free((pos.node -> value));
   free(pos.node);

   //Creates a position that is after the removed element.
   ListPos next = {.node = after};

   return next;



}

const unsigned char *list_inspect(ListPos pos)
{
   return pos.node -> value;
}
