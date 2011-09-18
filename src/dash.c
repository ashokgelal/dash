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

char* checkIfBackground(const char *line)
{
	char *delim = "&";
	char *temp = strdup(line);
	char *tok = strtok(temp, delim);

	if(tok==NULL || strcmp(tok, line)==0)
		return NULL;
	return tok;
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
}

int handleWait(char *bgTask){
	if(bgTask == NULL) {
		last_child_pid=waitpid(last_child_pid, &last_child_status, 0);
	}
	// background task
	else{
		addJob(last_child_pid, bgTask);
		last_child_pid = waitpid(last_child_pid, &last_child_status, WNOHANG);
	}

	if(last_child_pid == -1)
	{
		fprintf(stderr, "waitpid error\n");
		return EXIT_FAILURE;
	}

	if(WIFEXITED(last_child_status) || WIFSIGNALED(last_child_status))
	{
		fprintf(stderr, "waitpid success\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int handleCommand(const char *line)
{
	if(line==NULL || line=='\0' || strlen(line)==0)
		return 0;

	if(strcmp(line, "exit") == 0 || strcmp(line, "logout") == 0)
		return EXIT_SHELL;

	char *bgTask = checkIfBackground(line);

	if((last_child_pid = fork()) < 0)
		fprintf(stderr, "fork error\n");
	else if(last_child_pid == 0){
		char *param[2050];
		if(bgTask !=NULL) {
			param[0] = bgTask;
			param[1] = '\0';
		}
		else{
			parseParameters(line, param);
		}

		if(param[0]==NULL || strlen(param[0])==0)
			return EXIT_FAILURE;

		execvp(param[0], param);
		fprintf(stderr, "couldn't run %s\n", line);
		exit(127);
	}
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
	return EXIT_SUCCESS;
}
