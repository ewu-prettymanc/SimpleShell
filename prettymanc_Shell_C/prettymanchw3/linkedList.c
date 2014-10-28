/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-06-14                 *
 ****************************/

#include "linkedList.h"

//------------------------------------------------------------------------------
void addFirst(Node * newNode, Node ** head)
{
  if( g_size >= MAXLEN && MAXLEN > 0)
    {
      /* delete one off the back of the list */
      deleteLast(head );
    }
  newNode->next = *head;
  *head = newNode;
  g_size ++;
}

//------------------------------------------------------------------------------
void addLast(Node * newNode, Node ** head )
{
  if( g_size >=  MAXLEN && MAXLEN > 0 )
    {
      /* delete one from the front of the list */
      deleteFirst(head );
    }

  *head = addLast_rec(newNode, *head);
  g_size++;
}

// recursive helper function for addLast
Node * addLast_rec(Node * newNode, Node * cur )
{
  if( cur == NULL )
    return newNode;

  cur->next = addLast_rec(newNode, cur->next);
  
  return cur;
}

//------------------------------------------------------------------------------
/*
printList – if the list is empty prints “Empty List” otherwise prints the 
list one per line in this format word:length
*/
void printList(Node * head)
{
  if( head == NULL ) 
    {
      printf("Empty List\n");
      return;
    }
  
  Node * cur  = head;
  int i=0;

  for(i=1; cur != NULL ; i++ )
    {
      fprintf(stdout,"%d ", i);
      printNode(stdout, cur);
      fprintf(stdout, "\n");
      cur = cur->next;
    }
}
//------------------------------------------------------------------------------
void fprintList(Node * head, FILE * fin)
{
  /* The list is empty*/
  if( head == NULL ) 
    return;
    
  
  Node * cur  = head;
  while( cur != NULL )
    {
      printNode(fin, cur);
      fprintf(fin, "\n");
      cur = cur->next;
    }
}

//------------------------------------------------------------------------------
Node *  getElement(Node * cur, int index )
{
  if( index < 0 || index > MAXLEN || index > g_size )
    return NULL;
  
  if( index == 1)
    return cur;

  return getElement(cur->next, index-1 );
}

//------------------------------------------------------------------------------
Node * getLast(Node * cur )
{
  if( cur == NULL )
    return NULL;

  if( cur->next ==  NULL)
    return cur;

  return getLast(cur->next);
}

//------------------------------------------------------------------------------
int find(Node * lookfor, Node * head )
{
  Node * cur = head;
  int found = -1;

  while( cur != NULL && found != 0 )
    {
      found = compareToNode(lookfor, cur);
      cur = cur->next;
    }

  return found == 0 ? 1 : -1;
}


//------------------------------------------------------------------------------
int delete(Node * todelete, Node ** head )
{
  if( *head == NULL || todelete == NULL )
    return -1;
  
  int result = -1;

  *head = delete_rec( todelete, *head , &result);

  // Delete any duplicate nodes which may be present.
  if( result == 1 )
    delete(todelete, head );

  return result;
}

// This is a recursive helper function for deletion
Node * delete_rec( Node * todelete, Node * cur, int * result )
{
  Node * temp = NULL;

  if( cur == NULL )
    return NULL;

  if( compareToNode( todelete, cur ) == 0 )
    {
      *result=1;
      temp = cur->next;
      freeNode(cur);
      g_size--;
      return temp;
    }

  cur->next = delete_rec(todelete, cur->next, result);

  return cur;
}

//------------------------------------------------------------------------------
int deleteFirst(Node ** head )
{
  if( *head == NULL )
    return -1;
  
  Node * old_head = *head;
  *head = old_head->next;
  freeNode(old_head);
  g_size--;
  
  return 1;
}

//------------------------------------------------------------------------------
int deleteLast(Node ** head )
{
  if( *head == NULL )
    return -1;

  int result = -1;
  *head = deleteLast_rec(*head, &result);
  return result;
}

/* recursive helper function for deleteLast */
Node * deleteLast_rec(Node * cur, int * result )
{
  if( cur->next == NULL )
    {
      *result=1;
      freeNode(cur);
      g_size --;
      return NULL;
    }

  cur->next = deleteLast_rec(cur->next, result );
  return cur;
}

//------------------------------------------------------------------------------
void sortListAscending( Node ** head )
{
  int (*compareToptr)(Node *, Node *);
  compareToptr=&compareToNode;

  mergeSort(head, compareToptr);
}

//------------------------------------------------------------------------------
void sortListAscendingByLength(Node ** head )
{
  int (*compareToptr)(Node *, Node *);
  compareToptr=&compareToNodeLength;

  mergeSort(head, compareToptr);
}


//------------------------------------------------------------------------------
void mergeSort(Node ** head, int(*compareToptr)(Node*,Node*) )
{
  if( *head == NULL )
    return;

  if( (*head)->next == NULL )
    return;

  Node * a = NULL;
  Node * b = NULL;

  split(*head, &a, &b, 0);
  mergeSort(&a, compareToptr);
  mergeSort(&b, compareToptr);
  
  *head = merge(a, b, NULL, compareToptr);
  *head = reverse(*head, NULL);
}

//------------------------------------------------------------------------------
Node * merge(Node * a, Node * b, Node * res, int(*compareToptr)(Node*,Node*) )
{
  Node * newa = a;
  Node * newb = b;
  Node * old_res = res;
  int pulla = 1;

   if( a == NULL && b == NULL )
    return res;

   if( a == NULL )
    pulla = -1;
  else if ( b == NULL )
    pulla = 1;
  else if ( (*compareToptr)(a, b ) < 0 )
    pulla = 1;
  else
    pulla = -1;

   
  if( pulla == 1 )
    {
      res = a;
      newa = a->next;
      res->next=old_res;
    }
  else
    {
      res = b;
      newb = b->next;
      res->next = old_res;
    }

  return merge(newa, newb, res, compareToptr );
}

//------------------------------------------------------------------------------
Node * reverse(Node * n, Node * res )
{
  if( n == NULL )
    return res;
  
  Node * newn = n->next;
  Node * newres = n;

  newres->next = res;
  
  return reverse(newn, newres);
}

//------------------------------------------------------------------------------
void split(Node * n, Node ** a, Node ** b, int which )
{
  if( n == NULL )
    return;
  
  Node * storenext = n->next;
 
  if( (which % 2) == 0 )
    addFirst( n, a);
  else
    addFirst( n, b);

  split(storenext, a, b, which+1);

  n=NULL;
}

//------------------------------------------------------------------------------
void clearList(Node ** head)
{
  if( head == NULL )
    return;

  Node * next = NULL;

  while( *head != NULL )
    {
      next =  (*head)->next;
      freeNode(*head);
      *head = next;
      g_size--;
    }

  head = NULL; 
}

//------------------------------------------------------------------------------
int getListSize()
{
  return g_size;
}
