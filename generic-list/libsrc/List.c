
#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "ListIterator.h"

/*

	list.c 
		Contains functions to manipulate a doubly-linked list.
 
*/


/* private methods */

static NodePtr reverse(NodePtr);
static void print(NodePtr node, char * (*toString)(void *));

ListPtr createList(int(*getKey)(void *), 
                   char * (*toString)(void *),
				   void (*freeObject)(void *))
{
	ListPtr list;
	list = (ListPtr) malloc(sizeof(List));
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	list->getKey = getKey;
	list->toString = toString;
	list->freeObject = freeObject;
	return list;
}

void freeList(ListPtr list)
{
	init(list);
	while(hasNext(list))
		freeNode(next(list), list->freeObject);
	free(list);
}

int getSize(ListPtr list)
{
	if(list == NULL)
		return 0;
	return list->size;
}

Boolean isEmpty(ListPtr list)
{
	if(list == NULL) return TRUE;

	if (list->size == 0)
		return TRUE;
	else
		return FALSE;
}

void addAtFront(ListPtr list, NodePtr node)
{
	if (list == NULL) return;
	if (node == NULL) return;
	list->size++;
	node->next = list->head;
	node->prev = NULL;
	if (list->head == NULL)
	{
		list->head = node;
		list->tail = node;
	} else {
		list->head->prev = node;
		list->head = node;
	}
}

void addAtRear(ListPtr list, NodePtr node)
{
	if (list == NULL) return;
	if (node == NULL) return;
	list->size++;
	node->prev = list->tail;
	node->next = NULL;

	if(list->tail == NULL){
		list->head = node;
		list->tail = node;
	}else{
		list->tail->next = node;
		list->tail = node;
	}
}

NodePtr removeFront(ListPtr list)
{
	if(list == NULL) return NULL;
	if(list->size < 1) return NULL;

	NodePtr removedNode = list->head;
	if(list->head->next == NULL){
		list->head = NULL;
		list->tail = NULL;
	}else{
		list->head->next->prev = NULL;
		list->head = list->head->next;
	}

	removedNode->next = NULL;
	removedNode->prev = NULL;
	list->size--;
	return removedNode;
}

NodePtr removeRear(ListPtr list)
{
	if(list == NULL) return NULL;
	if(list->size < 1) return NULL;

	NodePtr removedNode = list->tail;
		if(list->tail->prev == NULL){
			list->head = NULL;
			list->tail = NULL;
		}else{
			list->tail->prev->next = NULL;
			list->tail = list->tail->prev;
		}

		removedNode->next = NULL;
		removedNode->prev = NULL;

		list->size--;
		return removedNode;
}

NodePtr removeNode(ListPtr list, NodePtr node)
{
	if(list == NULL) return NULL;
	if(node == NULL) return NULL;
	if(node==list->head) return removeFront(list);
	if(node==list->tail) return removeRear(list);

	init(list);
	Boolean found = FALSE;
	NodePtr oldNode;

	// search for the node first
	while(hasNext(list)){
		oldNode = next(list);
		if(oldNode == node){
			found = TRUE;
			break;
		}
	}

	if(found){
		node->prev->next = node->next;
		node->next->prev = node->prev;
		node->next = node->prev = NULL;
		list->size--;
		return node;
	}

	return NULL;
}

NodePtr search(ListPtr list, int key)
{
	if(list == NULL){
		return NULL;
	}

	init(list);
	NodePtr returnNode;
	while(hasNext(list))
	{
		returnNode = next(list);
		if(list->getKey(returnNode->obj) == key)
			return returnNode;
	}
	return NULL;
}

void reverseList(ListPtr list)
{
	if(list == NULL)
		return;

	list->tail = list->head;
	list->head  = reverse (list->head);
}

static NodePtr reverse(NodePtr L)
{
	NodePtr list = NULL;
	while (L != NULL) {
		NodePtr tmp = L;
		L = L->next;
		if (L != NULL) L->prev = tmp;
		tmp->next = list;
		tmp->prev = L;
		list = tmp;
	}
	return list;
}

void printList(ListPtr list)
{
	if (list) print(list->head, list->toString);
}

static void print(NodePtr node, char * (*toString)(void *))
{
	int count = 0;
	char *output;

	while (node) {
		output = (*toString)(node->obj);
		printf(" %s -->",output);
		free(output);
		node = node->next;
		count++;
		if ((count % 6) == 0)
			printf("\n");
	}
    printf(" NULL \n");
}
 

		

