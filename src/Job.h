/*
 * Job.h
 *
 *  Created on: Sep 17, 2011
 *      Author: ashokgelal
 */

#ifndef __JOB_H_
#define __JOB_H_
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define MAXPID_DIGITS 20
#define MAXJID_DIGITS 20

typedef struct job Job;
typedef struct job *JobPtr;
typedef enum { Running, Done } JobStatus;

struct job{
	pid_t p_id;
	uint job_id;
	char *command;
	JobStatus status;
	Boolean statusReported;
};

JobPtr createJob(pid_t, uint, char *);

char *toString(void *);
void freeJob(void *);
int getKey(void *);
char *toStringWithStatus(void *);
#endif /* __JOB_H_ */
