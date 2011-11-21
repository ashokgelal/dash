
#include <stdlib.h>
#include <stdio.h>
#include <List.h>
#include <Node.h>

int getKey(void * obj) 
{ 
	int *object = (int *) obj;
	return (int) *object; 
}

char *toString(void * obj) 
{ 
	int *object = (int *) obj;
	const int MAX_DIGITS = 10; /* maximum digits in an int */
	char *buf;
	buf = (char *) malloc(sizeof(char)*MAX_DIGITS);
	sprintf(buf, " %d ", *object);
	return buf;
}

void freeObject(void *obj) {free(obj);}

int main(int argc, char **argv)
{
	int i, n;
	ListPtr list;
	NodePtr node;
	int *object;

	n=1000;
	list = createList(getKey, toString, freeObject);
	for (i=0; i<n; i++) {
		object = (int *) malloc(sizeof(int));
		*object = i;
		node = createNode(object);
		addAtFront(list, node);
	}

	printList(list);
	freeList(list);
	exit(0);
}
