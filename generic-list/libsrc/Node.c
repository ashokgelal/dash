
#include "Node.h"


NodePtr createNode(void *obj)
{
	NodePtr newNode = (NodePtr) malloc (sizeof(Node));
	newNode->next = NULL;
	newNode->prev = NULL;
	newNode->obj = obj;
	return newNode;
}

void freeNode (NodePtr node, void (*freeObject)(void *))
{
	if (node == NULL) return;
	(*freeObject)(node->obj);
	free(node);
}

