/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#ifndef __PARSE_H__
#define __PARSE_H__

#include "ssh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "command.h"

//==============================================================================
int splitLocalCommand( char * s, char *** local);
int splitExternalCommands( cmd *** cmds, char * s );
int isValidCommand(char * s, int * numcmds);
int isValidTokenGroup(char * s, int * hasredirect);
int makeargs(char *s, char *** argv);
void printargs(int argc, char **argv);
void strip(char *s);
int getCharIndex(char * s, char c);
int isValidWord(char * w );
int isValidWordChar(char c );
int isValidOperator(char c );
int getRedirIndex(char * s );
int isValidRedirect(char * r );
int isValidRedirectChar(char c );
void freeArg(char ** arg, int* len );
//==============================================================================

#endif
