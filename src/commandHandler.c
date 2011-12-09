/*
 * commandHandler.c : handles entered command either in foreground or in background
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */

#include "commandHandler.h"

void handle_sigterm(int sig);

pid_t last_child_pid;
int last_child_status;
ListPtr jobList;
char *last_command;
int in_save, out_save;

/**
 * Frees up resources and quits the program with the provided status.
 */
static void quit(int status){
	dup2(in_save, STDIN_FILENO);
	dup2(out_save, STDOUT_FILENO);
	free(jobList);
	exit(status);
}

/**
 * Runs the given command in background.
 */
static void runInBackground(char *bgCommand){
	char *param[MAX_PARAMS];
	parseParameters(bgCommand, param);
	free(bgCommand);
	execvp(param[0], param);
	fprintf(stderr, "couldn't run background job: %s\n", param[0]);
	free(last_command);
	quit(127);
}

/**
 * Runs the given command in foreground
 */
static void runInForeground(char *command, char* param[]){
	// at this point we are sure that it is a foreground command
	execvp(param[0], param);
	goto finally;
	
	finally:
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
 * Handles bg command by sending a SIGCONT signal and changing the
 * status to Running.
 */
static void handleBgCommand(void){
	JobPtr job = findFirstStoppedJob(jobList);
	if (job == NULL)
		return;
	kill(job->p_id, SIGCONT);
	job->status = Running;
}

/**
 * Handles fg command for bringing the background job to foreground.
 * It first looks for a job with the given id. If job id is 0, it
 * looks for the last stopped job and brings it to back. If no stopped
 * job is found, it brings the latest background job to front.
 */
static pid_t handleFgCommand(int id){
	NodePtr node = findJobWithId(jobList, id);
	if(node == NULL)
		return -1;

	JobPtr job = (JobPtr)node->obj;
	kill(job->p_id, SIGCONT);
	job->status = Running;
	removeNode(jobList, node);
	last_command = job->command;
	return job->p_id;
}

/**
 * Main function for handling the provided line.
 * It parses the line first, identifies the type of commands, and executes accordingly
 */
char *param[MAX_PARAMS];
int handleCommand(const char *line) {
	// copy line so that we can modify it such as trimming the whitespaces etc
	last_command = malloc(strlen(line)+1);
	strcpy(last_command, line);
	last_command = trimwhitespace(last_command);
	if(*last_command==0)
	{
		free(last_command);
		return RETURN_SUCCESS;
	}
	int returnStatus = RETURN_FAIL;
	// if no bg task, bgCommand is set to null
	char *bgCommand = isBackgroundTask(last_command);
	if(isEmptyCommand(last_command)){
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	// don't parse parameters before testing for empty command
	parseParameters(last_command, param);

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

	if(isBgCommand(param[0])){
		handleBgCommand();
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	if (isFgCommand(param[0])){
		char * id = param[1];
		pid_t towait_pid = last_child_pid;
		if(id == NULL){
			id = "0";
		}

		int jobId = atoi(id);
		towait_pid = handleFgCommand(jobId);
		if (towait_pid == -1){
			// no job on stack
			if(jobId == 0)
				fprintf(stdout, "dash: fg: current: no such job\n");
			else
				fprintf(stdout, "dash: fg: %d: no such job\n", jobId);
			returnStatus = RETURN_SUCCESS;
			goto finally;
		}

		fprintf(stdout, "%s\n", last_command);
		last_child_pid = towait_pid;
		int waitStatus;
		waitStatus=waitpid(last_child_pid, &last_child_status, WSTOPPED);
		if(waitStatus == -1) {
			returnStatus = RETURN_FAIL;
			goto finally;
		}
		if(WIFSTOPPED(last_child_status)){
			returnStatus = RETURN_SUCCESS;
			goto finally;
		}
	}

	if((last_child_pid = fork()) < 0){
		fprintf(stderr, "fork error\n");
		returnStatus = RETURN_FAIL;
		goto finally;
	}

	else if(last_child_pid == 0) {
		last_child_pid = getpid();
		handleNormalCommand(last_command, param, bgCommand);
		// we should never be at this point
		returnStatus = RETURN_FAIL;
		goto finally;
	}else{
		if(setpgid(last_child_pid,last_child_pid) == -1){
			fprintf(stderr, "couldn't run foreground job: %s\n", last_command);
			goto finally;
		}
	}

	if(bgCommand == NULL) {
		waitpid(last_child_pid, &last_child_status, WCONTINUED | WUNTRACED);
		goto finally;
		if(WIFCONTINUED(last_child_status) || WIFSTOPPED(last_child_status)) {
			returnStatus = RETURN_SUCCESS;
			goto finally;
		}
	}else {
		addJob(jobList, last_child_pid, bgCommand, Running);
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	finally:
		if(getSize(jobList)>0)
			jobList = reportCompletedJobs(jobList);
		free(param[0]);
		free(last_command);
		free(bgCommand);
		return returnStatus;
}

/**
 * Runs the dash program with the provided prompt.
 */
int run(char *prompt){
	(void) signal(SIGCHLD, SIG_IGN);
	(void) signal(SIGTTOU, SIG_IGN);
	(void) signal(SIGTTIN, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGINT, handle_sigterm);
	(void) signal(SIGTSTP, handle_sigterm);

	char *line;
	jobList = createList(getKey, toString, freeJob);
	in_save = dup(STDIN_FILENO);
	out_save = dup(STDOUT_FILENO);
	using_history();
	while((line=readline(prompt))) {
		if(handleCommand(line)==EXIT_SHELL) {
			free(line);
			break;
		}
		dup2(in_save, STDIN_FILENO);
		dup2(out_save, STDOUT_FILENO);
		add_history(line);
		free(line);
	}

	// catch for Ctrl-D
	if(line == '\0')
		free(line);

	// free pending jobs as we are exiting
	init(jobList);
	while(hasNext(jobList)) {
		NodePtr node = next(jobList);
		freeNode(node, freeJob);
	}
	free(jobList);
	dup2(in_save, STDIN_FILENO);
	dup2(out_save, STDOUT_FILENO);
	return EXIT_SUCCESS;
}

/**
 * Handler for signals. It only handles Ctrl+Z and Ctrl+C signals.
 */
void handle_sigterm(int sig){
	switch(sig){
	case SIGTSTP:
		kill(last_child_pid, SIGSTOP);
		addJob(jobList, last_child_pid, last_command, Stopped);
		return;
	case SIGINT:
		kill(last_child_pid, SIGKILL);
		fprintf(stdout, "\n");
		return;
	}
}
