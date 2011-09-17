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
#include "version.h"
#include "constants.h"

char *prompt;
pid_t last_child_pid;
int last_child_status;

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

int handleCommand(char *line)
{
	if(line==NULL || line=='\0')
		return 0;

	if(strcmp(line, "exit") == 0 || strcmp(line, "logout") == 0)
	{
		return EXIT_SHELL;
	}

	if((last_child_pid = fork()) < 0)
		fprintf(stderr, "fork error\n");
	else if(last_child_pid == 0){
		execlp(line, line, (char *)0);
		fprintf(stderr, "couldn't run %s\n", line);
		exit(127);
	}
	if((last_child_pid=waitpid(last_child_pid, &last_child_status, 0)) <0)
		fprintf(stderr, "waitpid error\n");

	return 0;
}

int main(int argc, char *argv[]) {
	char *line;
	verifyShowVersionCommand(&argc, argv);
	clearScreen();
	setupPrompt();
	using_history();
	while((line=readline(prompt))) {
		if(handleCommand(line)==EXIT_SHELL) {
			free(line);
			break;
		}
		//printf("%s\n", line);
		add_history(line);
		free(line);
	}
	//TODO: free up line properly
	clearScreen();
	return EXIT_SUCCESS;
}
