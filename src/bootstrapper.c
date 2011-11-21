/*
 * bootstrapper.c
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <common.h>
#include "constants.h"
#include "utilities.h"
#include "version.h"
#include "commandHandler.h"

/**
 * Returns the prompt to be displayed.
 * This first checks if ENV_PROMPT environment variable is set. If not
 * it return default prompt defined in constants.h
 */
static char * getPrompt(){
	char * prompt = getenv(ENV_PROMPT);
	if(prompt==NULL || *prompt =='\0')
		prompt = DEFAULT_PROMPT;
	return prompt;
}

/**
 * Returns TRUE if the program argument contains v for displaying version
 */
static Boolean isShowVersionCommand(int *argc, char *argv[])
{
	int opt;
	opt = getopt(*argc, argv, ":v");

	if(opt == -1)
		return FALSE;

	if(opt == 'v')
		return TRUE;
	else{
		fprintf(stderr, "Usage: %s [-v]\n", PROGNAME);
		exit(EXIT_FAILURE);
	}

	return FALSE;
}

/**
 * Initializes dash by first checking the version, clearing the console and
 * setting up the title of the console. It then returns the prompt for dash.
 */
char * initDash(int argc, char *argv[]) {
	if(isShowVersionCommand(&argc, argv)) {
		fprintf(stdout, "%s version: %s\n", PROGNAME, getVersion());
		exit(EXIT_SUCCESS);
	}

	//handleCommand("clear");
	setupConsoleTitle();
	return getPrompt();
}
