/*
 * JobControl.c : handles job
 *
 *  Created on: Sep 18, 2011
 *      Author: Ashok Gelal
 */

#include <sys/wait.h>
#include "JobControl.h"

/**
 * Checks and prints the status of all the DONE (and running) jobs for
 * the given list.
 * It returns a new list that contains still running jobs.
 * If printRunning is set TRUE, it prints the information of all the Running
 * jobs.
 */
static ListPtr reportStatus(ListPtr list, Boolean printRunning){
	ListPtr newList = createList(getKey, toString, freeJob);
	init(list);

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

/**
 * Reports all jobs in the background that are running or done but whose status
 * has not been yet reported.
 */
ListPtr reportAllJobs(ListPtr list){
	ListPtr newList = reportStatus(list, TRUE);
	free(list);
	return newList;
}

/**
 * Reports all completed jobs in the background who status has not yet
 * been reported.
 */
ListPtr reportCompletedJobs(ListPtr list) {
	ListPtr newList = reportStatus(list, FALSE);
	free(list);
	return newList;
}

/**
 * Creates a job with given pid, and command, and adds the job
 * to the given list.
 */
void addJob(ListPtr list, pid_t pid, char *command, JobStatus status) {
	int nextid = 0;
	// try to get the next highest no.
	if(getSize(list)>0) {
		JobPtr lastJob = (JobPtr)list->tail->obj;
		nextid = lastJob->job_id;
	}

	JobPtr job = createJob(pid, ++nextid, command, status);
	NodePtr node = createNode(job);
	addAtRear(list, node);
	char *jobStr = toString(job);
	fprintf(stdout, "\n%s\n", jobStr);
	free(jobStr);
}


JobPtr findFirstStoppedJob(ListPtr list) {
	initReverse(list);
	NodePtr node;
	while(hasNextReverse(list)){
		node = nextReverse(list);
		JobPtr job = (JobPtr) node->obj;
		if (job->status == Stopped){
			return job;
		}
	}
	return NULL;
}

NodePtr findJobWithId(ListPtr list, int id){
	// TODO move this
	initReverse(list);
	NodePtr node;
	while(hasNextReverse(list)){
		node = nextReverse(list);
		JobPtr job = (JobPtr) node->obj;

		if (job->job_id == id){
			return node;
		}
		if (id==0 && job->status == Stopped){
			return node;
		}
	}

	// we don't have any stopped jobs
	// if id == 0, try to find the first job job that is not STOPPED
	if (id == 0)
		return list->tail;
	return NULL;
}
