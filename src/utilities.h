/*
 * utilities.h
 *
 *  Created on: Sep 19, 2011
 *      Author: ashokgelal
 */

#ifndef __UTILITIES_H_
#define __UTILITIES_H_
#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <fcntl.h>
#include "constants.h"

char cur_path[4096];
char *trimwhitespace(char *);
void parseParameters(const char *, char *[]);
void setupConsoleTitle(void);
char *getHomePath(void);
char* isBackgroundTask(const char *line);
Boolean isExitCommand(const char * );
Boolean isEmptyCommand(const char *);
Boolean isJobsCommand(const char *);
Boolean isChdirCommand(char *);
Boolean isBgCommand(char *);
int isFgCommand(char *);


#endif /* __UTILITIES_H_ */
