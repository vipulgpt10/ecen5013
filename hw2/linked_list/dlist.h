/**
 * @file dlist.h
 * @brief An abtraction for doubly linked list functions
 *
 * This header file provides the prototypes of the function for
 * various doubly linked list operations.
 *
 * @author Vipul Gupta
 * @date 03 February 2018
 *
 */

#ifndef __DLIST_H__
#define __DLIST_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


/* Doubly linked list node containing prev and next pointers */
struct dlNode
{
	uint32_t data;
	struct dlNode * prev;
	struct dlNode * next;
};

/**
 * @brief Inserts a new node at the beginning of the list
 *
 * Given the reference pointer to the head of the list, it will insert 
 * a new node at the beginnning of the list and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head 
 *
 * @return Pointer to the head node
 */
struct dlNode * insert_at_beginning(struct dlNode ** head_ref, 
									uint32_t ndata);

/**
 * @brief Inserts a new node at the end of the list
 *
 * Given the reference pointer to the head of the list, it will 
 * insert a new node at the end of the list and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head 
 *
 * @return Pointer to the head node
 */
struct dlNode * insert_at_end(struct dlNode ** head_ref, uint32_t ndata);

/**
 * @brief Inserts a new node at the given index of the list (0 = head)
 *
 * Given the pointer to the head node of the list, it will insert a new node 
 * at the given index of the list and returns the head node pointer.
 *
 * @param head Head node pointer
 *
 * @return Pointer to the head of the list
 */
struct dlNode * insert_at_position(struct dlNode * head, 
									uint32_t ndata, uint32_t index);

/**
 * @brief Deletes a node from the beginning of the list
 *
 * Given the reference pointer to the head of the list, it will delete a node 
 * from the beginnning of the list and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head
 *
 * @return Pointer to the head of the list
 */
struct dlNode * delete_from_beginning(struct dlNode ** head_ref);

/**
 * @brief Deletes a node from the beginnig of the list
 *
 * Given the reference pointer to the head of the list, it will delete a node 
 * from the end of the list and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head
 *
 * @return Pointer to the head of the list
 */
struct dlNode * delete_from_end(struct dlNode ** head_ref);

/**
 * @brief Deletes a node from the given index of the list (0 = head)
 *
 * Given the reference pointer to the head of the list, it will delete a node 
 * from the given index of the list and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head
 *
 * @return Pointer to the head of the list
 */
struct dlNode * delete_from_position(struct dlNode ** head_ref, 
										uint32_t index);

/**
 * @brief Size of the linked list 
 *
 * Given the pointer to the head node of the list, it will return the
 * of links in the list.
 *
 * @param head_ref Head node pointer
 *
 * @return Pointer to the head of the list
 */
size_t list_size(struct dlNode * head);

/**
 * @brief Destroys all the nodes in the list
 *
 * Given the reference pointer to the head of the list, it will free 
 * up the memory by deleting all the nodes and returns the head node pointer.
 *
 * @param head_ref Reference pointer to the head 
 *
 * @return Pointer to the head node
 */
struct dlNode * destroy(struct dlNode ** head_ref);


#endif /* __DLIST_H__ */