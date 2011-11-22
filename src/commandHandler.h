/*
 * commandHandler.h
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */

#ifndef __COMMANDHANDLER_H_
#define __COMMANDHANDLER_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <common.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <signal.h>

#include <List.h>
#include <Node.h>
#include "JobControl.h"
#include "Job.h"
#include "constants.h"
#include "utilities.h"
int handleCommand(const char *);
int run(char *);

#endif /* __COMMANDHANDLER_H_ */
