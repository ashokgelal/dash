/*
 * utilities.c
 *
 *  Created on: Sep 19, 2011
 *      Author: ashokgelal
 */

#include <string.h>
#include <ctype.h>
#include "utilities.h"
#include "constants.h"

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
