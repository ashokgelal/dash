/*
 * Job.c
 *
 *  Created on: Sep 17, 2011
 *      Author: ashokgelal
 */

#include "Job.h"

JobPtr createJob(int p_id, char *command)
{
	static uint job_id = 1;
	JobPtr newJob = (JobPtr) malloc(sizeof(Job));
	newJob->p_id = p_id;
	newJob->job_id = job_id;
	newJob->command = (char *)malloc(sizeof(char)*(strlen(command)+1));
	strcpy(newJob->command, command);
	job_id++;
	return newJob;
}

int getKey(void *obj)
{
	JobPtr myjob = (JobPtr) obj;
	return myjob->job_id;
}

char *toString(void *obj)
{
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

