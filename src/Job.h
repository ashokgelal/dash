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

struct job{
	int p_id;
	int job_id;
	char *command;
};

JobPtr createJob(int, char *);

char *toString(void *);
void freeJob(void *);
int getKey(void *);

#endif /* __JOB_H_ */
