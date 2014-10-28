/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#include "Node.h"

//------------------------------------------------------------------------------
void freeNode(Node * n )
{
  freeData(n->data);
  free(n);
  n= NULL;
}


//------------------------------------------------------------------------------
void printNode(FILE * stream , Node * n )
{
  if( n == NULL )
    {
      fprintf(stream, "(nil)");
      return;
    }

  printData(stream, n->data);
}

//------------------------------------------------------------------------------
int compareToNode(Node * lookfor, Node * cur )
{
  return compareToData(lookfor->data, cur->data);
}

//------------------------------------------------------------------------------
int compareToNodeLength(Node * lookfor, Node * cur )
{
  return compareToDataLength(lookfor->data, cur->data);
}
