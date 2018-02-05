/**
 * @file dlist.c
 * @brief The implementation file for doubly linked list functions
 *
 * This implementation file provides the function definitions for
 * various doubly linked list operations.
 *
 * @author Vipul Gupta
 * @date 03 February 2018
 *
 */


#include "dlist.h"


struct dlNode * insert_at_beginning(struct dlNode ** head_ref, uint32_t ndata)
{
	struct dlNode * newNode = (struct dlNode *)malloc(sizeof(struct dlNode));

	newNode->data = ndata;
	newNode->prev = NULL;		
	newNode->next = *head_ref;

	/* Check if list is empty */
	if((*head_ref) != NULL)
		(*head_ref)->prev = newNode;

	*head_ref = newNode;

	return (*head_ref);
}

struct dlNode * insert_at_end(struct dlNode ** head_ref, uint32_t ndata)
{
	struct dlNode * current = *head_ref;

	struct dlNode * newNode = (struct dlNode *)malloc(sizeof(struct dlNode));

	newNode->data = ndata;
	newNode->next = NULL;

	/* Check if list is empty */
	if((*head_ref) == NULL)
	{
		newNode->prev = NULL;
		*head_ref = newNode;
		return (*head_ref);
	}

	while(current->next != NULL)
		current = current->next;

	current->next = newNode;
	newNode->prev = current;

	return (*head_ref);
}

struct dlNode * insert_at_position(struct dlNode * head, 
								uint32_t ndata, uint32_t index)
{
	struct dlNode * current = head, * tmp;

	struct dlNode * newNode = (struct dlNode *)malloc(sizeof(struct dlNode));

	while(index--)
		current = current->next;

	tmp = current->next;

	current->next = newNode;
	newNode->prev = current;

	newNode->data = ndata;

	if(tmp != NULL)
	{
		newNode->next = tmp;
		tmp->prev = newNode;
	}
	else
		newNode->next = NULL;

	return head;
}

struct dlNode * delete_from_beginning(struct dlNode ** head_ref)
{
	struct dlNode * current = *head_ref;

	current = current->next;
	
	free(current->prev);

	if(current != NULL)
		current->prev = NULL;

	*head_ref = current;

	return (*head_ref);
}

struct dlNode * delete_from_end(struct dlNode ** head_ref)
{
	struct dlNode * current = *head_ref, * tmp;

	while(current->next != NULL)
		current = current->next;

	tmp = current->prev;

	/* If current is the head node */
	if(current == (*head_ref))
		*head_ref = NULL;
	else
		tmp->next = NULL;

	free(current);

	return (*head_ref);
}

struct dlNode * delete_from_position(struct dlNode ** head_ref, uint32_t index)
{
	struct dlNode * current = *head_ref, * tmp_p, * tmp_n;

	if(index == 0)
		return (delete_from_beginning(head_ref));

	while(index--)
		current = current->next;

	tmp_n = current->next;
	tmp_p = current->prev;

	if(tmp_n != NULL)
	{
		tmp_p->next = tmp_n;
		tmp_n->prev = tmp_p;
	}
	else
		tmp_p->next = NULL;

	free(current);

	return (*head_ref);
}

size_t list_size(struct dlNode * head)
{
	size_t cnt = 0;

	while(head != NULL)
	{
		head = head->next;
		cnt++;
	}

	return cnt;
}

struct dlNode * destroy(struct dlNode ** head_ref)
{
	
	size_t cnt = list_size(*head_ref);
	size_t i;
	
	for(i = 0; i < cnt; i++)
	{
		*head_ref = delete_from_end(head_ref);
	}

	return  (*head_ref);
}