/*
 * JobControl.h
 *
 *  Created on: Sep 18, 2011
 *      Author: ashokgelal
 */

#ifndef JOBCONTROL_H_
#define JOBCONTROL_H_
#include <List.h>
#include <Node.h>
#include <ListIterator.h>
#include <ReverseListIterator.h>
#include <sys/wait.h>
#include "Job.h"

ListPtr reportAllJobs(ListPtr);
ListPtr reportCompletedJobs(ListPtr);
void addJob(ListPtr, pid_t, char *, JobStatus);
JobPtr findFirstStoppedJob(ListPtr);
NodePtr findJobWithId(ListPtr, int);

#endif /* JOBCONTROL_H_ */
