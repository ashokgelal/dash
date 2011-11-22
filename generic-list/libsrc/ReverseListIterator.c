/*
 * ReverseListIterator.c
 * Reverse Iterator pattern for a list
 * Created on: Aug 28, 2011
 *      Author: Ashok Gelal
 */

#include "ReverseListIterator.h"

static NodePtr current = NULL;
static uint size = 0;

/**
 * Verifies the structural integrity of a list.
 * This catches conditions such as a node being removed while
 * iterating. When the size of the given list is not equals to
 * the size which gets initialized the first time this iterator is
 * created, the program blows up. Be careful!
 */
static void verifyConcurrentModification(ListPtr list)
{
	if(size != list->size)
	{
		fprintf(stderr, "Concurrent Modification to a List is not permitted");
		exit(1);
	}
}

/**
 * Initializes an iterator.
 */
void initReverse(ListPtr list) 
{
	if(list == NULL) return;
	current = list->tail;
	size = list->size;
}

/**
 * Returns true if a list has more nodes
 */
Boolean hasNextReverse(ListPtr list)
{
	verifyConcurrentModification(list);
	if(current == NULL)
		return FALSE;
	return TRUE;
}

/**
 * Returns the previous node from a list
 */
NodePtr nextReverse(ListPtr list)
{
	if(list == NULL) return NULL;
	verifyConcurrentModification(list);
	NodePtr returnNode = current;
	current = current->prev;
	return returnNode;
}




