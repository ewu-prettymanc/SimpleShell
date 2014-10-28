/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-22-14                 *
 ****************************/

#ifndef __SSH_H__
#define __SSH_H__

#include "linkedList.h"
#include "command.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
//==============================================================================
#define MAX 102 /* NOTE: +2 ---> counting null byte and newline */
#define MAXTEMP 4096
#define HISTFILE ".ssh_history"

//==============================================================================
int runssh();
int runAsExternal(char * input, Node ** historyList );
//int runCommand( cmd * cmd , int numcmds, int curcmd, int * fds,
// int numpipes, int curpipe);
int runAsLocal(char * input, Node ** historyList );
int changeDirectory(char ** newdir, int len,  int reset );
void grabNextCommand( char * cwd,  char * input, FILE * fin );
char * getCWD(char * cwd );
Node * initHistory(Node ** history );
void saveHistory(Node * history );
void saveCommandToHistory(char * command );
void addHistory(char * s, Node ** history );
Node * createNode(char * buffer );
//==============================================================================

#endif
