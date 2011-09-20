/*
 * utilities.h
 *
 *  Created on: Sep 19, 2011
 *      Author: ashokgelal
 */

#ifndef __UTILITIES_H_
#define __UTILITIES_H_
#include <common.h>
char cur_path[4096];
char *trimwhitespace(char *);
void parseParameters(const char *, char *[]);
void setupConsoleTitle();
char *getHomePath();
char* isBackgroundTask(const char *line);
Boolean isExitCommand(const char * );
Boolean isEmptyCommand(const char *);
Boolean isJobsCommand(const char *);
Boolean isChdirCommand(char *);


#endif /* __UTILITIES_H_ */
