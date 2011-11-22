/*
 * bootstrapper.h
 *
 *  Created on: Sep 19, 2011
 *      Author: ashokgelal
 */

#ifndef __BOOTSTRAPPER_H_
#define __BOOTSTRAPPER_H_
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <common.h>
#include "constants.h"
#include "utilities.h"
#include "version.h"
#include "commandHandler.h"

char * initDash(int , char *[]);
void cleanup(void);

#endif /* __BOOTSTRAPPER_H_ */
