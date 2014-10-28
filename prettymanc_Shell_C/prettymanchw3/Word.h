/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#ifndef __WORD_H__
#define __WORD_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
typedef struct
{
  char * word;
  int length;
} Word;

//------------------------------------------------------------------------------
void freeData(Word * w );
void printData(FILE * stream , Word * w );
int compareToData(Word * lookfor, Word * cur );
int compareToDataLength(Word * lookfor, Word * cur );

#endif
