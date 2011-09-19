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
#include <pwd.h>

#include <List.h>
#include <Node.h>

#include "version.h"
#include "constants.h"
#include "Job.h"
#include "JobControl.h"

char *prompt;
CommandType last_command_type;
char *last_command;
pid_t last_child_pid;
int last_child_status;
ListPtr jobList;
char cur_path[4096];

static void verifyShowVersionCommand(int *argc, char *argv[])
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

static void setupPrompt(){
	prompt = getenv(ENV_PROMPT);
	if(prompt==NULL || *prompt =='\0')
		prompt = DEFAULT_PROMPT;
}


static char *trimwhitespace(char *str) {
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

static char* isBackgroundTask(const char *line) {
	char *delim = "&";
	char *temp = strdup(line);
	char *tok = strtok(temp, delim);

	if(tok==NULL || strcmp(tok, line)==0)
		return NULL;
	return trimwhitespace(tok);
}

static void parseParameters(const char *line, char *params[]) {
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

static void addJob(pid_t pid, char *command) {
	JobPtr job = createJob(pid, command);
	NodePtr node = createNode(job);
	addAtRear(jobList, node);
	char *jobStr = toString(job);
	fprintf(stdout, "%s\n", jobStr);
	free(jobStr);
}

static Boolean isExitCommand(const char * command) {
	return (strcmp(command, "exit") == 0 || strcmp(command, "logout") == 0);
}

static Boolean isEmptyCommand(const char *command) {
	return (command==NULL || command=='\0' || strlen(command)==0);
}

static Boolean isJobsCommand(const char *command) {
	return (strcmp(command, "jobs")==0);
}

static void quit(int status){
	free(jobList);
	exit(status);
}

static void runInBackground(char *bgCommand){
	char *param[MAX_PARAMS];
	parseParameters(bgCommand, param);
	execvp(param[0], param);
	fprintf(stderr, "couldn't run background job: %s\n", bgCommand);
	// we are quitting; so free memory
	free(bgCommand);
	quit(127);
}

static Boolean isChdirCommand(char *command){
	return (strcmp(command, "cd")==0);
}

static void runInForeground(char *command, char* param[]){
	// at this point we are sure that it is a foreground command
	execvp(param[0], param);
	fprintf(stderr, "couldn't run foreground job: %s\n", command);
	// we are quitting; so free memory
	free(command);
	quit(127);
}

static void handleNormalCommand(char *command, char *param[], char *bgCommand){
	if(bgCommand != NULL)
		runInBackground(bgCommand);
	else {
		// if it is background we will never be here
		free(bgCommand);
		runInForeground(command, param);
	}
}

static char *getHomePath(){
	uid_t id = getuid();

	struct passwd *pwd;
	pwd = getpwuid(id);

	return (pwd == NULL) ? "." : pwd->pw_dir;
}

static void setupConsole(){
	getcwd(cur_path, 4096);
	printf("%c]0;%s%s%c",'\033', "Olet tässä: ",cur_path, '\007');
}

static int handleCommand(const char *line) {
	// copy line so that we can modify it such as trimming the whitespaces etc
	char *command = malloc(strlen(line));
	strcpy(command, line);
	command = trimwhitespace(command);
	int returnStatus = RETURN_FAIL;
	// if no bg task, bgCommand is set to null
	char *bgCommand = isBackgroundTask(command);
	char *param[MAX_PARAMS];
	if(isEmptyCommand(command)){
		jobList = reportCompletedJobs(jobList);
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
		//fprintf(stderr, "path is: %s", path);
		if(path=='\0') {
			path = getHomePath();
			//fprintf(stderr, "path is: %s", path);
		}
		int status = chdir(path);
		if(status==0){
			setupConsole();
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
		addJob(last_child_pid, bgCommand);
		returnStatus = RETURN_SUCCESS;
		goto finally;
	}

	finally:
		free(command);
		free(bgCommand);
		return returnStatus;
}


static void setupJobList(){
	jobList = createList(getKey, toString, freeJob);
}

static void clearScreen(){
	handleCommand("clear");
}

int main(int argc, char *argv[]) {
	char *line;
	verifyShowVersionCommand(&argc, argv);
	clearScreen();
	setupPrompt();
	setupConsole();
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
	quit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}
