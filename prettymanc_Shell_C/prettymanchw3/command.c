/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-22-14                 *
 ****************************/

#include "command.h"

//------------------------------------------------------------------------------
void cleancmds(cmd **c, int * len)
{
  if( *len <=0 || c == NULL)
    return;

  int i=0;

  for( i=0; i < *len; i++)
    cleancmd(c[i]);

  c=NULL;
  free(c);
  *len;
} /* end cleancmds */

//------------------------------------------------------------------------------
void cleancmd(cmd * c )
{
  if( c == NULL )
    return;
  
  int i=0;
  
  if( c->cmdlength >= 0 && c->command != NULL)
    {
      for( i=0; i < c->cmdlength+1; i++)
	free(c->command[i]);
      free(c->command);
    }
  
  if( c->redirlength >= 0 && c->redirects != NULL)
    {
      for( i=0; i < c->redirlength+1; i++)
	free(c->redirects[i]);
      free(c->redirects);
    }
  free(c);
}/* end cleancmd */

//------------------------------------------------------------------------------
void printcmds(cmd ** c, int len )
{
  int i=0;

  for( i=0; i < len; i++)
    printcmd(c[i]);
}/* print cmds */

//------------------------------------------------------------------------------
/* NOTE: This doesn't format the commands perfectly, more or fewer
   spaces may be present
   Also note how the last element of each array contains a null element 
*/
void printcmd(cmd * c )
{
  int i=0;
  
  for( i=0; i < c->cmdlength; i++)
    printf("%s ", c->command[i]);
    
  for( i=0; i < c->redirlength; i++)
    printf("%s ", c->redirects[i]);
    
  printf("\n");
}/* end print cmd */

