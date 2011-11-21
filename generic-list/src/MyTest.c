
/*
 * File: MyTest.c
 * Author: Ashok Gelal
 * Runs bunch of tests on LinkedList and Iterator
 */
#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <Node.h>
#include <List.h>
#include <ListIterator.h>

#include "Object.h"
int failTestsCount;
int totalTestsCount;

/**
 * Keeps track of total tests run and tests failed
 * hasPassed - pass TRUE if a test passed, FALSE otherwise
 */
void adjustFailCounter(Boolean hasPassed)
{
	totalTestsCount++;
	if(hasPassed == FALSE)
		failTestsCount++;
}

/**
 * Tests an uninitialized list. Please note that to get away with segfault headache (hell?)
 * my list doesn't break when you try add remove from an empty list and so do isEmpty and getSize
 * methods. The consequence of this is that whether an a list is uninitialized or empty, isEmpty()
 * always returns TRUE and getSize() always returns 0.
 *
 * Also note that because this is a test for an uninitialized list (and node), it is natural to get
 * some compiler warnings. Warnings verifies that this test is not fake and hence
 * doesn't deserve to get penalized some points or whispering in someone's ear that I write code with
 * bunch of warnings :)
 */
void testUninitializedList()
{
	ListPtr list;
	NodePtr node;

	// isEmpty shouldn't crash on an uninitialized list
	Boolean retBol = isEmpty(list);
	adjustFailCounter(retBol);
	// getSize shouldn't crash on an uninitialized list
	int retCount = getSize(list);
	adjustFailCounter(retCount==0 ? TRUE : FALSE);

	// addFront on an uninitialized list
	addAtFront(list, node);
	// made it so far means we survived above call
	adjustFailCounter(TRUE);
	addAtRear(list, node);
	// made it so far means we survived above call
	adjustFailCounter(TRUE);

	NodePtr retNode = removeFront(list);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = removeRear(list);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = removeNode(list, node);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = search(list, 0);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);
}

/**
 * Tests an empty list. Again, please ignore the compile time warnings
 */
void testEmptyList(){
	ListPtr list = createList(getKey, toString, freeObject);
	NodePtr node;

	Boolean retBol = isEmpty(list);
	adjustFailCounter(retBol);

	int retCount = getSize(list);
	adjustFailCounter(retCount==0 ? TRUE : FALSE);

	NodePtr retNode = removeFront(list);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = removeRear(list);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = removeNode(list, node);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);

	retNode = search(list, 0);
	adjustFailCounter(retNode==NULL ? TRUE : FALSE);
	freeList(list);
}

/**
 * Tests a non-empty list. This is a really simple test (although it looks complicated)
 * which plays with just one node by adding and removing to a list.
 */
void testNonEmptyList(){
	ListPtr list = createList(getKey, toString, freeObject);
	ObjectPtr job = createObject(0, "foo");
	NodePtr node = createNode(job);
	addAtFront(list, node);

	Boolean retBol = isEmpty(list);
	adjustFailCounter(retBol == FALSE ? TRUE : FALSE);

	int retCount = getSize(list);
	adjustFailCounter(retCount==1 ? TRUE : FALSE);

	NodePtr retNode = search(list, 0);
	adjustFailCounter(retNode == node ? TRUE : FALSE);

	retNode = removeFront(list);
	adjustFailCounter(retNode==node ? TRUE : FALSE);
	retCount = getSize(list);
	adjustFailCounter(retCount==0 ? TRUE : FALSE);
	freeNode(retNode, freeObject);

	job = createObject(0, "foo");
	node = createNode(job);
	addAtFront(list, node);

	retNode = removeRear(list);
	adjustFailCounter(retNode==node ? TRUE : FALSE);
	retCount = getSize(list);
	adjustFailCounter(retCount==0 ? TRUE : FALSE);
	freeNode(retNode, freeObject);

	job = createObject(0, "foo");
	node = createNode(job);
	addAtFront(list, node);

	retNode = removeNode(list, node);
	adjustFailCounter(retNode==node ? TRUE : FALSE);
	retCount = getSize(list);
	adjustFailCounter(retCount==0 ? TRUE : FALSE);
	freeNode(retNode, freeObject);
	freeList(list);
}

/**
 * Creates a bunch of nodes and tests that they can be properly added.
 */
void testListWithMultipleNodes(){
	ListPtr tempList = createList(getKey, toString, freeObject);
	ObjectPtr job1 = createObject(0, "foo1");
	NodePtr node1 = createNode(job1);
	addAtRear(tempList, node1);

	ObjectPtr job2 = createObject(1, "foo1");
	NodePtr node2 = createNode(job2);
	addAtFront(tempList, node2);

	ObjectPtr job3 = createObject(3, "foo3");
	NodePtr node3 = createNode(job3);
	addAtRear(tempList, node3);

	Boolean retBol = isEmpty(tempList);
	adjustFailCounter(retBol == FALSE ? TRUE : FALSE);

	int retCount = getSize(tempList);
	adjustFailCounter(retCount==3 ? TRUE : FALSE);
	freeList(tempList);
}

/**
 * Tests the list iterator. A good side effect of this test is it
 * also tests the positions of nodes i.e. it verifies each node is added to
 * proper position to the list.
 */
void testListIterator(){
	ListPtr tempList = createList(getKey, toString, freeObject);
	ObjectPtr job1 = createObject(0, "foo1");
	NodePtr node1 = createNode(job1);
	addAtRear(tempList, node1);

	ObjectPtr job2 = createObject(1, "foo1");
	NodePtr node2 = createNode(job2);
	addAtFront(tempList, node2);

	ObjectPtr job3 = createObject(3, "foo3");
	NodePtr node3 = createNode(job3);
	addAtRear(tempList, node3);

	int i = 1;
	init(tempList);
	NodePtr currentNode;
	while(hasNext(tempList)){
		currentNode = next(tempList);
		switch (i) {
			case 1:
				adjustFailCounter(currentNode==node2 ? TRUE : FALSE);
				break;
			case 2:
				adjustFailCounter(currentNode==node1 ? TRUE : FALSE);
				break;
			case 3:
				adjustFailCounter(currentNode==node3 ? TRUE : FALSE);
				break;
			default:
				// we should not reach this point
				adjustFailCounter(FALSE);
		}
		i++;
	}

	freeList(tempList);
}

int main(int argc, char **argv)
{
	failTestsCount = 0;
	totalTestsCount = 0;

	testUninitializedList();
	testEmptyList();
	testNonEmptyList();
	testListWithMultipleNodes();
	testListIterator();

	fprintf(stderr,"Total tests failed %d out of: %d\n", failTestsCount, totalTestsCount);
	exit(0);
}




