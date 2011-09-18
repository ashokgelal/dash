/*
 ============================================================================
 Name        : dash.c
 Author      : Ashok Gelal
 Version     :
 Copyright   : All Wrongs Reserved
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>

#include <List.h>
#include <Node.h>

#include "version.h"
#include "constants.h"
#include "Job.h"
#include "JobControl.h"

char *prompt;
pid_t last_child_pid;
int last_child_status;
ListPtr jobList;

void verifyShowVersionCommand(int *argc, char *argv[])
{
	int opt;
	opt = getopt(*argc, argv, ":v");

	if(opt == -1)
		return;

	// we have some command line options
	if(opt == 'v'){
		printf("%s version: %s\n", PROGNAME, getVersion());
		exit(EXIT_SUCCESS);
	}
	else{
		fprintf(stderr, "Usage: %s [-v]\n", PROGNAME);
		exit(EXIT_FAILURE);
	}
}

void setupPrompt(){
	prompt = getenv(ENV_PROMPT);
	if(prompt==NULL || *prompt =='\0')
		prompt = DEFAULT_PROMPT;
}

void clearScreen(){
	if((last_child_pid = fork()) < 0)
		fprintf(stderr, "fork error");
	else if(last_child_pid == 0){
		execlp("clear", "clear", (char *)0);
		fprintf(stderr, "couldn't clear the screen");
		exit(127);
	}
	if((last_child_pid=waitpid(last_child_pid, &last_child_status, 0)) <0)
		fprintf(stderr, "waitpid error");
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

char* checkIfBackground(const char *line)
{
	char *delim = "&";
	char *temp = strdup(line);
	char *tok = strtok(temp, delim);

	if(tok==NULL || strcmp(tok, line)==0)
		return NULL;
	return trimwhitespace(tok);
}

void parseParameters(const char *line, char *params[])
{
	char *delim = " ";
	char *temp = strdup(line);
	int i = 0;
	params[i++] = strtok(temp, delim);

	while(1) {
		char * tok = strtok(NULL, delim);
		params[i++] = tok;
		if(tok == NULL)
			break;
	}
}
void addJob(pid_t pid, char *command) {
	JobPtr job = createJob(pid, command);
	NodePtr node = createNode(job);
	addAtRear(jobList, node);
	char *jobStr = toString(job);
	fprintf(stdout, "%s\n", jobStr);
	free(jobStr);
}

int handleWait(char *bgTask){
	if(bgTask == NULL) {
		last_child_pid=waitpid(last_child_pid, &last_child_status, 0);

		if(WIFEXITED(last_child_status) || WIFSIGNALED(last_child_status)) {
			return EXIT_SUCCESS;
		}
	}
	// background task
	else{
		addJob(last_child_pid, bgTask);
		return EXIT_SUCCESS;
		//last_child_pid = waitpid(last_child_pid, &last_child_status, WNOHANG);
		//fprintf(stdout, "status of last job: %d\n", last_child_status);
	}

	if(last_child_pid == -1) {
		fprintf(stderr, "waitpid error\n");
	}

	return EXIT_FAILURE;
}

int handleCommand(const char *line) {
	// copy line so that we can modify it such as trimming the whitespaces etc
	char *command = malloc(strlen(line));
	strcpy(command, line);

	if(command==NULL || command=='\0' || strlen(command)==0){
		jobList = reportCompletedJobs(jobList);
		free(command);
		return RETURN_SUCCESS;
	}

	if(strcmp(trimwhitespace(command), "exit") == 0 || strcmp(trimwhitespace(command), "logout") == 0){
		free(command);
		return EXIT_SHELL;
	}

	if(strcmp(trimwhitespace(command), "jobs")==0) {
		jobList = reportAllJobs(jobList);
		free(command);
		return RETURN_SUCCESS;
	}

	char *bgTask = checkIfBackground(command);

	if((last_child_pid = fork()) < 0)
		fprintf(stderr, "fork error\n");
	else if(last_child_pid == 0){
		char *param[2050];
		if(bgTask !=NULL) {
			parseParameters(bgTask, param);
		}
		else{
			parseParameters(command, param);
		}

		if(param[0]==NULL || strlen(param[0])==0) {
			free(command);
			return RETURN_SUCCESS;
		}

		execvp(param[0], param);
		fprintf(stderr, "couldn't run %s\n", command);
		exit(127);
	}
	free(command);
	return handleWait(bgTask);
}

void setupJobList(){
	jobList = createList(getKey, toString, freeJob);
}

int main(int argc, char *argv[]) {
	char *line;
	verifyShowVersionCommand(&argc, argv);
	clearScreen();
	setupPrompt();
	setupJobList();
	using_history();
	while((line=readline(prompt))) {
		if(handleCommand(line)==EXIT_SHELL) {
			free(line);
			break;
		}
		add_history(line);
		free(line);
	}
	clearScreen();
	freeList(jobList);
	exit(EXIT_SUCCESS);
}
