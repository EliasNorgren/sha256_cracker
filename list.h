/**
 * @defgroup List
 *
 * @brief The module is an double linked list.
 *
 * (Detailed description of the module.)
 *
 * This is an implemented doubly linked list with an head that points to its
 * end and beginning.
 *
 * It contains different functions to operate the list.
 *
 *
 *
 * @author  Elias Norgren
 * @since   2019-11-12
 *
 * @{
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/**
 * @brief The type for a node in the list.
 *
 * Next points to the next node in the list.
 * Prev points to the previous node in the list.
 * Value contains a string.
 *
 */
struct node
{
    struct node *next;
    struct node *prev;
    char *value;
};


/**
 * @brief The type for a list.
 *
 * Head will keep track of the last and first node in the list.
 * Its head value will be unused.
 *
 */
typedef struct list
{
    struct node head;
} List;

/**
 * @brief The type for a list position.
 *
 * This will contain a pointer to a node to keep track of a position in the list.
 *
 */
typedef struct list_pos
{
    struct node *node;
} ListPos;

/**
 * @brief Create and return an empty list.
 *
 *  Allocates memory for a list. Makes its next/prev pointers point to its
 *  own head.
 *
 *  It is the callers responsibility to deallocate the list and its nodes
 *  by calling list_destroy().
 *
 * @param            -
 *
 * @return           Pointer to a list.
 */
List *list_create(void);

/**
 * @brief Deallocate the list (and all of its values, if any).
 *
 *  Removes and deallocates all of the nodes and their values in the list.
 *  Deallocates the List pointer.
 *
 * @param lst        Pointer to a list.
 *
 * @return           -
 */
void list_destroy(List *lst);

/**
 * @brief Check if the list is empty.
 *
 *  Checks if the heads next/prev pointer both point to the same head. If so
 *  then there are no elements in the list.
 *
 * @param lst        Constant pointer to List
 *
 * @return           true if empty. False if not empty.
 */
bool list_is_empty(const List *lst);

//
/**
 * @brief Returns the position of the first element.
 *
 *  Returns the position of where head.next points wich is the first position.
 *
 * @param lst        Pointer to list.
 *
 * @return           ListPos
 */
ListPos list_first(List *lst);

/**
 * @brief Get the position after(!) the last element.
 *
 *  Returns the position of the last element wich also is the adress to the head.
 *
 * @param lst        Pointer to list.
 *
 * @return           ListPos
 */
ListPos list_end(List *lst);

/**
 * @brief Check if two ListPos points to the same node.
 *
 * Checks if the nodes in the two ListPos points to the same node. Returns
 * true if they do, else false.
 *
 * @param p1        ListPos
 * @param p2        ListPos
 * @return           Bool
 */
bool list_pos_equal(ListPos p1, ListPos p2);

/**
 * @brief Forward to the next position.
 *
 *  Returns the position of the pos -> node.next.
 *
 * @param pos        ListPos
 *
 * @return           ListPos
 */
ListPos list_next(ListPos pos);

/**
 * @brief Backward to the previous position.
 *
 *  Returns the position of the pos -> node.prev.
 *
 * @param pos        ListPos
 *
 * @return           ListPos
 */
ListPos list_prev(ListPos pos);

/**
 * @brief Insert the value before the position and return the position of the
 new element.
 *
 *  Makes a new node and inserts the input string onto it. The the node is
 *  placed before the given position and is linked in the list.
 * Returns the position of the new element.
 *
 * @param pos        ListPos
 * @param value      const char*
 * @return           ListPos of the new elemeent.
 */
ListPos list_insert(ListPos pos, const char *value);

/**
 * @brief Remove the value at the position and return the position of the
 next element.
 *
 *  Links together the nodes inbetween the node that are to be removed.
 *  Frees the memory of the value in the node and the node.
 *
 * @param pos        ListPos
 *
 * @return           ListPos
 */
ListPos list_remove(ListPos pos);

/**
 * @brief Get the value at the position.
 *
 *  Returns the string that is allocated in the current node.
 *
 * @param pos        ListPos
 *
 * @return           const char *
 */
const char *list_inspect(ListPos pos);

/**
 * @}
 */

#endif /* LIST_H */
