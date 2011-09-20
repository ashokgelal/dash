/*
 * commandHandler.c : handles entered command either in foreground or in background
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */
#include <stdlib.h>
#include <unistd.h>
#include <common.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <stdio.h>

#include <List.h>
#include <Node.h>

#include "commandHandler.h"
#include "JobControl.h"
#include "Job.h"
#include "constants.h"
#include "utilities.h"

CommandType last_command_type;
char *last_command;
pid_t last_child_pid;
int last_child_status;
ListPtr jobList;

/**
 * Frees up resources and quits the program with the provided status.
 */
static void quit(int status){
	free(jobList);
	exit(status);
}

/**
 * Runs the given command in background.
 */
static void runInBackground(char *bgCommand){
	char *param[MAX_PARAMS];
	parseParameters(bgCommand, param);
	execvp(param[0], param);
	fprintf(stderr, "couldn't run background job: %s\n", bgCommand);
	// we are quitting; so free memory
	free(bgCommand);
	quit(127);
}

/**
 * Runs the given command in foreground
 */
static void runInForeground(char *command, char* param[]){
	// at this point we are sure that it is a foreground command
	execvp(param[0], param);
	fprintf(stderr, "couldn't run foreground job: %s\n", command);
	// we are quitting; so free memory
	free(command);
	quit(127);
}

/**
 * Identifies the type of given command (either foreground or background)
 * and calls them accordingly.
 * command: original command
 * param[]: parameters for the given command
 * bgCommand: if NULL, represents a background command
 */
static void handleNormalCommand(char *command, char *param[], char *bgCommand){
	if(bgCommand != NULL)
		runInBackground(bgCommand);
	else {
		// if it is background we will never be here
		free(bgCommand);
		runInForeground(command, param);
	}
}

/**
 * Main function for handling the provided line.
 * It parses the line first, identifies the type of commands, and executes accordingly
 */
int handleCommand(const char *line) {
	// copy line so that we can modify it such as trimming the whitespaces etc
	char *command = malloc(strlen(line));
	strcpy(command, line);
	command = trimwhitespace(command);
	int returnStatus = RETURN_FAIL;
	// if no bg task, bgCommand is set to null
	char *bgCommand = isBackgroundTask(command);
	char *param[MAX_PARAMS];
	if(isEmptyCommand(command)){
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	// don't parse parameters before testing for empty command
	parseParameters(command, param);

	if(isExitCommand(param[0])){
		returnStatus = EXIT_SHELL;
		goto finally;
	}

	if(isJobsCommand(param[0])){
		jobList = reportAllJobs(jobList);
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	if(isChdirCommand(param[0])){
		char *path = param[1];
		if(path=='\0') {
			path = getHomePath();
		}
		int status = chdir(path);
		if(status==0){
			setupConsoleTitle();
			returnStatus = RETURN_SUCCESS;
		}
		returnStatus = (status==0) ? RETURN_SUCCESS : RETURN_FAIL;
		goto finally;
	}

	if((last_child_pid = fork()) < 0){
		fprintf(stderr, "fork error\n");
		returnStatus = RETURN_FAIL;
		goto finally;
	}

	else if(last_child_pid == 0) {
		handleNormalCommand(command, param, bgCommand);
		// we should never be at this point
		returnStatus = RETURN_FAIL;
		goto finally;
	}

	if(bgCommand == NULL) {
		last_child_pid=waitpid(last_child_pid, &last_child_status, 0);
		if(last_child_pid == -1) {
			fprintf(stderr, "waitpid error\n");
			returnStatus = RETURN_FAIL;
			goto finally;
		}
		if(WIFEXITED(last_child_status) || WIFSIGNALED(last_child_status)) {
			returnStatus = RETURN_SUCCESS;
			goto finally;
		}
	}else {
		addJob(jobList, last_child_pid, bgCommand);
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	finally:
		if(getSize(jobList)>0)
			jobList = reportCompletedJobs(jobList);
		free(command);
		free(bgCommand);
		return returnStatus;
}

/**
 * Runs the dash program with the provided prompt.
 */
int run(char *prompt){
	char *line;
	jobList = createList(getKey, toString, freeJob);
	using_history();
	while((line=readline(prompt))) {
		if(handleCommand(line)==EXIT_SHELL) {
			free(line);
			break;
		}
		add_history(line);
		free(line);
	}

	// catch for Ctrl-D
	if(line == '\0')
		free(line);

	handleCommand("clear");
	quit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
