/*
 * utilities.c : contains all the utilities functions which, if required,
 * can be used by multiple source files.
 *
 *  Created on: Sep 19, 2011
 *      Author: Ashok Gelal
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include "utilities.h"
#include "constants.h"

/**
 * Trims whitespace off a string and returns it.
 * I've grabbed this piece of code from stackoverflow.com
 */
char *trimwhitespace(char *str) {
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

/**
 * Parses a line and sets up parameters array.
 * The first element of the params is always the name of
 * the command itself
 */
void parseParameters(const char *line, char *params[]) {
	char *delim = " ";
	char *temp = strdup(line);
	int i = 0;
	params[i++] = trimwhitespace(strtok(temp, delim));

	while(1) {
		char * tok = strtok(NULL, delim);
		params[i++] = tok;
		// test for null and boundary
		if(tok == NULL || i==MAX_PARAMS)
			break;
	}
}

/**
 * Sets up the title of the the console so to show the current path.
 * Note: Olet tässä is just to make Linus happy
 */
void setupConsoleTitle(){
	getcwd(cur_path, 4096);
	printf("%c]0;%s%s%c",'\033', "Olet tässä: ",cur_path, '\007');
}

/**
 * Returns the home path of the current user.
 */
char *getHomePath(){
	uid_t id = getuid();

	struct passwd *pwd;
	pwd = getpwuid(id);

	return (pwd == NULL) ? "." : pwd->pw_dir;
}

/**
 * Checks if the given command is a background task (task with an &).
 * This ignores all the characters after '&' symbol.
 * Returns a string which resembles the actual command without '&'
 */
char* isBackgroundTask(const char *line) {
	char *delim = "&";
	char *temp = strdup(line);
	char *tok = strtok(temp, delim);

	if(tok==NULL || strcmp(tok, line)==0)
		return NULL;
	return trimwhitespace(tok);
}

/**
 * Returns TRUE if the given command is an exit command - either 'exit' or 'logout'
 */
Boolean isExitCommand(const char * command) {
	return (strcmp(command, "exit") == 0 || strcmp(command, "logout") == 0);
}

/**
 * Returns TRUE if the given command is an empty command
 */
Boolean isEmptyCommand(const char *command) {
	return (command==NULL || command=='\0' || strlen(command)==0);
}

/**
 * Returns TRUE if the given command is a 'jobs' command
 */
Boolean isJobsCommand(const char *command) {
	return (strcmp(command, "jobs")==0);
}

/**
 * Returns TRUE if the given command is a change directory command 'chdir'
 */
Boolean isChdirCommand(char *command){
	return (strcmp(command, "cd")==0);
}
