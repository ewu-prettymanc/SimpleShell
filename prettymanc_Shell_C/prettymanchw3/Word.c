/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#include "Word.h"

//------------------------------------------------------------------------------
void freeData(Word * w )
{
  int length = 0;
  free(w->word);
  free(w);
  w = NULL;
}

//------------------------------------------------------------------------------
void printData(FILE * stream , Word * w )
{
  if( w == NULL )
    {
      printData(stream,"(nil)");
      return;
    }

  //fprintf(stream, "%s:%d",w->word,w->length );
  fprintf(stream, "%s",w->word );
}

//------------------------------------------------------------------------------
int compareToData(Word * lookfor, Word * cur )
{
  return strcmp(lookfor->word, cur->word);
}

//------------------------------------------------------------------------------
int compareToDataLength(Word * lookfor, Word * cur )
{
  int diff =  (lookfor->length - cur->length);

  if( diff == 0 )
    return compareToData(lookfor, cur);
  
  return diff;
}
