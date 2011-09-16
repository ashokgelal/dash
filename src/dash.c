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
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "version.h"

#define PROGNAME "dash"

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

int main(int argc, char *argv[]) {
	char *line;
	char *prompt;
	prompt = getenv("DASH_PROMPT");
	if(prompt==NULL || *prompt =='\0')
		prompt = ":) ";

	verifyShowVersionCommand(&argc, argv);
	using_history();
	while((line=readline(prompt))) {
		printf("%s\n", line);
		add_history(line);
		free(line);
	}
	return EXIT_SUCCESS;
}
