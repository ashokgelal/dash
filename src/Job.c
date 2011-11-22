/*
 * Job.c : Represents a background task
 *
 *  Created on: Sep 17, 2011
 *      Author: ashokgelal
 */

#include "Job.h"

/**
 * Creates a job with given pid, jobid, and command
 */
JobPtr createJob(pid_t p_id, uint job_id, char *command, JobStatus status) {
	JobPtr newJob = (JobPtr) malloc(sizeof(Job));
	newJob->p_id = p_id;
	newJob->job_id = job_id;
	newJob->command = (char *)malloc(sizeof(char)*(strlen(command)+1));
	strcpy(newJob->command, command);
	newJob->status = status;
	newJob->statusReported = FALSE;
	return newJob;
}

/**
 * Returns the key of this task
 */
int getKey(void *obj) {
	JobPtr myjob = (JobPtr) obj;
	return myjob->job_id;
}

/**
 * Returs a properly formatted job task
 */
char *toString(void *obj) {
	JobPtr myjob = (JobPtr)obj;
	char *temp = (char *)malloc(sizeof(char)*strlen(myjob->command)+1+MAXPID_DIGITS+MAXJID_DIGITS+5);
	sprintf(temp, "[%d] %d %s", myjob->job_id, myjob->p_id, myjob->command);
	return temp;
}

void freeJob(void *obj){
	JobPtr myjob = (JobPtr)obj;
	if(myjob == NULL) return;
	free(myjob->command);
	free(myjob);
}

/**
 * Returns a formatted job object with status
 */
char *toStringWithStatus(void *obj){
	JobPtr myjob = (JobPtr) obj;
	char *status;
	if(myjob->status == Running)
		status = "Running";
	else if(myjob->status == Stopped)
		status = "Stopped";
	else
		status = "Done";
	char *temp = (char *)malloc(sizeof(char)*strlen(myjob->command)+1+MAXJID_DIGITS+strlen(status)+5);
	sprintf(temp, "[%d] %s %s", myjob->job_id, status, myjob->command);
	return temp;
}
