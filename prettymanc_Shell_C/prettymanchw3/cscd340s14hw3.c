/****************************
 * Colton Prettyman         *
 * Lab3 OpSys 340 EWU       *
 * Professor Stuart Steiner *
 * 04-12-14                 *
 ****************************/

#include <stdio.h>
#include <stdlib.h>
#include "ssh.h"

//==============================================================================
int main(int argc, char ** argv)
{
  if( runssh() < 0)
    fprintf(stderr, "Error running Simple Shell!\n");
  else
    fprintf(stdout, "Exiting Simple Shell!\n");
    
} // end main
