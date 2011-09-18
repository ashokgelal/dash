/*
 * ListIterator.h
 *
 *  Created on: Aug 28, 2011
 *      Author: Ashok Gelal
 */

#ifndef __LISTITERATOR_H
#define __LISTITERATOR_H
#include <stdio.h>
#include "common.h"
#include "Node.h"
#include "List.h"
void init(ListPtr L);
Boolean hasNext(ListPtr L);
NodePtr next(ListPtr L);

#endif /* __LISTITERATOR_H */
