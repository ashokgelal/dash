/*
 ============================================================================
 Name        : dash.c
 Author      : Ashok Gelal
 Version     :
 Copyright   : All Wrongs Reserved
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void) {
	char *s;
	s = getenv("DASH_PROMPT");
	if(s==NULL || *s =='\0')
		s = ":) ";
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
