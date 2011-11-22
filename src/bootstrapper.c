/*
 * bootstrapper.c
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */

#include "bootstrapper.h"

pid_t last_shell_pid;
/**
 * Returns the prompt to be displayed.
 * This first checks if ENV_PROMPT environment variable is set. If not
 * it return default prompt defined in constants.h
 */
static char * getPrompt(void){
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
static void setupProcessGroup(void){
	last_shell_pid = tcgetpgrp(STDIN_FILENO);	// NEW captures terminal's current pgid
	tcsetpgrp(STDIN_FILENO, getpgrp());			// NEW sets teminal's pgid to process's currents pgid
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

	setbuf(stdout, NULL);
	setupProcessGroup();
	handleCommand("clear");
	setupConsoleTitle();
	return getPrompt();
}

/*
 * Cleans up first by resetting tcpgrp to previous shell and then clearing the screen
 */
void cleanup(void){
	tcsetpgrp(STDIN_FILENO, last_shell_pid);
	handleCommand("clear");
}
