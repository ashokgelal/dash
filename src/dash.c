/*
 ============================================================================
 Name        : dash.c
 Author      : Ashok Gelal
 Copyright   : All Wrongs Reserved
 dash.c : main executable file
 ============================================================================
 */

#include "bootstrapper.h"
#include "commandHandler.h"

int main(int argc, char *argv[]) {
	// bootstrap dash
	char * prompt = initDash(argc, argv);

	// loop-eval-execute = run
	return run(prompt);
}
