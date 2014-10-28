/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-22-14                 *
 ****************************/

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//==============================================================================
typedef struct
{
  /* Holds the 2D array of command in [0] and args thereafter, 
     \0 terminating every array 
  */
  char ** command;
  int cmdlength; /* The number of commands */
  /* Each redirect is followed by what it is being redirected to. 
     ie. redirects[0] == >, Then redirects[1] contains the output name
  */
  char ** redirects;
  int redirlength;
} cmd;

//------------------------------------------------------------------------------
void cleancmds(cmd**c, int * len);
void cleancmd(cmd * c );
void printcmds(cmd ** c, int len );
void printcmd(cmd * c );
//------------------------------------------------------------------------------

#endif
