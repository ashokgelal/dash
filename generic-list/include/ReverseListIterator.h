/*
 * ReverseListIterator.h
 *
 *  Created on: Aug 28, 2011
 *      Author: Ashok Gelal
 */

#ifndef __REVERSELISTITERATOR_H
#define __REVERSELISTITERATOR_H
#include <stdio.h>
#include "common.h"
#include "Node.h"
#include "List.h"
void initReverse(ListPtr L);
Boolean hasNextReverse(ListPtr L);
NodePtr nextReverse(ListPtr L);

#endif /* __REVERSELISTITERATOR_H */
