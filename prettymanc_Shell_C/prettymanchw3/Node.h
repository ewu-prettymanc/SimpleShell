/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#ifndef __NODE_H__
#define __NODE_H__

#include <stdio.h>
#include "Word.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
typedef struct
{
  void * data;
  struct Node * next;
} Node;

//------------------------------------------------------------------------------
void freeNode(Node * n );
void printNode(FILE * stream , Node * n );
int compareToNode(Node * lookfor, Node * cur );
int compareToNodeLength(Node * lookfor, Node * cur );

#endif
