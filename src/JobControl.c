/*
 * JobControl.c
 *
 *  Created on: Sep 18, 2011
 *      Author: ashokgelal
 */

#include <sys/wait.h>
#include "JobControl.h"

static ListPtr reportStatus(ListPtr list, Boolean printRunning){
	init(list);
	ListPtr newList = createList(getKey, toString, freeJob);

	while(hasNext(list)) {
		NodePtr node = next(list);
		JobPtr job = (JobPtr) node->obj;
		job->statusReported = TRUE;
		int status;
		if(job->status != Done && waitpid(job->p_id, &status, WNOHANG)!=0) {
			job->status = Done;
			char * jobStr = toStringWithStatus(job);
			fprintf(stdout, "%s\n", jobStr);
			free(jobStr);
		}
		else{
			if(printRunning) {
				char * jobStr = toStringWithStatus(job);
				fprintf(stdout, "%s\n", jobStr);
				free(jobStr);
			}
			// copy to new list
			addAtRear(newList, node);
		}
	}

	return newList;
}

ListPtr reportAllJobs(ListPtr list){
	ListPtr newList = reportStatus(list, TRUE);
	free(list);
	return newList;
}

ListPtr reportCompletedJobs(ListPtr list) {
	ListPtr newList = reportStatus(list, FALSE);
	free(list);
	return newList;
}

void addJob(ListPtr list, pid_t pid, char *command) {
	int nextid = 0;
	// try to get the next highest no.
	if(getSize(list)>0) {
		JobPtr lastJob = (JobPtr)list->tail->obj;
		nextid = lastJob->job_id;
	}

	JobPtr job = createJob(pid, ++nextid, command);
	NodePtr node = createNode(job);
	addAtRear(list, node);
	char *jobStr = toString(job);
	fprintf(stdout, "%s\n", jobStr);
	free(jobStr);
}
