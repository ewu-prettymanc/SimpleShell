/****************************
 * Colton Prettyman         *
 * Hw1 OpSys 340 EWU        *
 * Professor Stuart Steiner *
 * 04-22-14                 *
 ****************************/

#include "parse.h"

//------------------------------------------------------------------------------
/*
This function attempts to split the input command into a local command. Meaning
a command that is not forked or piped such as. 
cd (..) 
history
! (#)
!!
If it can parse a local command from the input it will place these commands into local
and return >0. If not it will set local to NULL and the length to 0, and return < 0.
NOTE: This function mallocs memory dynamically and local must be freed. 
The integer return value is the number of elements in **local
*/
int splitLocalCommand( char * s, char *** local)
{
  if( s == NULL )
    return -1;
  
  /* Check the case for cd -args- */
  if( strncmp( s, "cd", strlen("cd")) == 0 )
    if( s[strlen("cd")] == '\0' || s[strlen("cd")] == ' ' )
      return makeargs(s, local);

  /* Check for history */
  if( strcmp(s,"history")==0)
    return makeargs(s,local);

  /* Check for !# */
  if( s[0] == '!' )
    {
      int alldigits=-1;
      int i=0;
      for(i=0; s[i]!= '\0' ; i++)
	{
	  alldigits=isdigit(s[1]);
	  if( alldigits <= 0 )
	    break;
	}
      
      if(alldigits > 0)
	return makeargs(s,local);
    } /* end if */

  /* Check for !! */
  if( strcmp(s,"!!") == 0 )
    return makeargs(s,local);
  
  return -1;
}/* end splitLocalCommand*/

//------------------------------------------------------------------------------
/* This function splits the input string into an array of command structures. This 
structure contains all data needed. The return value of this function is the length
of the array of command structure pointers. Also this value is the number
of commands parsed out correctly from the input string. If the return value is <= 0.
Then there are no commands and an error should be handled appropriately elsewhere
*/
int splitExternalCommands( cmd *** cmds, char * s )
{
  int i=0;
  int numcmds = 0;
  /* Validify the command fist */
  if( isValidCommand(s, &numcmds) < 0 )
    return -1;


  *cmds = (cmd**)calloc(numcmds, sizeof(cmd*));
  if( !*cmds ){
    perror("calloc: ");
    exit(EXIT_FAILURE);
  }
  
  for(i=0; i < numcmds; i++)
    {
      (*cmds)[i] = (cmd*)calloc(1,sizeof(cmd));
      if( !(*cmds)[i]){
	perror("calloc: ");
	exit(EXIT_FAILURE);
      }
    }
  
  strip(s);
  char * copy = (char*)calloc(strlen(s)+1,sizeof(char));
  if( ! copy ){
    perror("calloc: ");
    exit(EXIT_FAILURE);
  }
  strcpy(copy,s);
  char * saveptr=NULL;
  char * token = NULL;
  char temp[MAX];
  int redirIndex=-1;
  char * regex=NULL;

  /* now parse the command into each cmd element */
   if( numcmds > 1 )
     regex = "|"; /* piped command */
   else
     regex = "\0"; /*singlet command */

   token = strtok_r(copy,regex, &saveptr);
   
   for(i=0; i < numcmds && token != NULL; i++)
     {
       redirIndex = getRedirIndex(token);
       
       
       if(redirIndex >= 0 ) /* There are redirections...parse them*/
	 {
	   strcpy(temp, token);
	   strcpy(temp, (token+redirIndex));
	   
	   (*cmds)[i]->redirlength = makeargs(temp, &((*cmds)[i]->redirects) ); 
	   
	   /* Mask off the last half of the string for the commands */
	   token[redirIndex-1]='\0';
	 }
       else
	 { (*cmds)[i]->redirlength=0; }

       (*cmds)[i]->cmdlength=makeargs(token, &((*cmds)[i]->command));

       token = strtok_r(NULL,regex, &saveptr);
     }
 
  free(copy);
  return numcmds;
} /* end splitExternalCommands */

//------------------------------------------------------------------------------
int isValidCommand(char * s, int * numcmds)
{
  strip(s);
  char * copy = (char*)calloc(strlen(s)+1,sizeof(char));
  if( ! copy ){
    perror("calloc: ");
    exit(EXIT_FAILURE);
  }
  strcpy(copy,s);
  char * saveptr=NULL;
  char * nexttoken=NULL;
  char * token = strtok_r(copy,"|", &saveptr);
  int validgroup = -1;
  int gotLastSpace =-1;
  int gotFirstSpace = -1;
  int cmdcount=0;
  int hasredirect = -1;

  if( strchr(s, '|' ) != NULL ) /* It contains a pipe!*/
    {
      /*Make sure the | is not the first or last character */
      if( s[0] != '|' &&  s[strlen(s)-1] != '|' )
	{
	  
	  /* pull out tokegroups while they are available*/
	  do {
	    if( token == NULL )
	      break;

	    /* Not the first command...make sure there is a leading space */
	    /* There should be a space at the beggining left from the |"space"*/
	    if(cmdcount > 0 ) 
	      gotFirstSpace =  token[0] == ' ' ? 1 : -1; 
	    else
	      gotFirstSpace = 1;
	      
	    /* Not the last command...make sure there is a trailing space */
	    nexttoken = strtok_r(NULL, "|", &saveptr);
	    if( nexttoken != NULL )
	      gotLastSpace = token[strlen(token)-1] == ' ' ? 1 : -1;
	    else
	      gotLastSpace = 1;

	    /* A space was not detected on both sides of the pipe!!*/
	    if( (validgroup=gotLastSpace) < 0 || ((validgroup=gotFirstSpace) < 0))
	      break;
	      
	    validgroup  = isValidTokenGroup(token, &hasredirect);
	    
	    /* The user is trying to have redirects and pipes */
	    if( hasredirect > 0 && nexttoken != NULL )
	      validgroup = -1;

	    cmdcount++;
	    token = nexttoken;
	  } while( validgroup > 0 );
	}
    }
  else /* only one command entered no pipe */
    {
      validgroup = isValidTokenGroup(s, &hasredirect);
      cmdcount=1;
    }
  
  if( validgroup < 0 )
    cmdcount=0;
  *numcmds = cmdcount;

  free(copy);
  return validgroup;
} /* end isValidCommand*/

//------------------------------------------------------------------------------
/* This function returns >0 if the token group is valid and < 0 if it is invalid. 
A valid token group is as spefied. 
-Command--Arguments-->redirection<--output/input filename-- 
--Word-----Word---------< or > ------single word-----------
*/
int isValidTokenGroup(char * s , int * hasredirect)
{
  // Create copies of s so that the integrity of s is preserved
  char * copy = (char*)calloc(strlen(s)+1,sizeof(char));
  if( ! copy ){
    perror("calloc: ");
    exit(EXIT_FAILURE);
  }
  strcpy(copy,s);

  char * token = strtok(copy, " ");
  int gotredirect=-1;
  int result = -1;
  int gotfile = -1;
  int validredirect=-1;

  do { 
    if( token == NULL )
      break;
      
    if( (gotredirect = isValidRedirect(token)) > 0)  
      break;
	
    result =  isValidWord(token);
    token = strtok(NULL, " ");
  } while( result > 0 );

  *hasredirect = gotredirect;

  /* There was an invalid command, or valid command but no 
     redirection in the input */
  if( result < 0 || (result > 0 && gotredirect < 0 )  || token == NULL) {
    free(copy);
    return result;
  }
  
  /* There was redirection...make sure it is valid */
  token = strtok(NULL," " );
  if( token == NULL ) {
    free(copy);
    return -1;
  }
  
  gotfile = isValidWord(token);
  
  validredirect = gotfile<=gotredirect ? gotfile : gotredirect;
  
  /* keep pulling out redirects until there are none...*/
  while( validredirect > 0 )
    {
      token = strtok(NULL, " " );
      if( token == NULL )
	break;
      /* Try to grab another redirect operator */
      if( ( gotredirect = isValidRedirect(token) ) < 0 ){
	validredirect = gotredirect;
	break;
      }
      
      /* The operator was found now find the word/file after it */
      token = strtok(NULL, " " );
      if( token == NULL ){
	validredirect = -1;
	break;
      }

      gotfile = isValidWord(token);
      
      validredirect = gotfile<=gotredirect ? gotfile : gotredirect;
    }
  
  free(copy);
  return validredirect;
}

//------------------------------------------------------------------------------
/* Returns the index of the first occurance of c in s.
   if c does not occur at all then -1 is returned 
*/
int getCharIndex(char * s, char c)
{
  int i=0;

  for(i=0; i < strlen(s); i++)
    if( s[i] == c)
      return i;

  return -1;
} /* end getCharIndex*/

//------------------------------------------------------------------------------
int makeargs(char *s, char *** argv)
{
  strip(s);
  // Create copies of s so that the integrity of s is preserved
  char * copy = (char*)calloc(strlen(s)+1,sizeof(char));
  if( ! copy ){
    perror( "calloc: ");
    exit(EXIT_FAILURE);
  }
     
  strcpy(copy,s);

  char * token = strtok(copy, " ");
  int argc=0;
  int i=0;

  /* The command has no spaces!*/
  for( argc = 0; token != NULL; argc++)
    token=strtok(NULL," ");
  
  // The first array will point to the start of the second.
  *argv = (char**)calloc(argc+1, sizeof(char*));
  if( ! *argv ){
    perror( "calloc: ");
    exit(EXIT_FAILURE);
  }
  

  // reset the tokenizer
  strcpy(copy,s);
  token = strtok(copy," ");
  
  for( i=0; i< argc; i++)
    {
      (*argv)[i] = (char*) calloc(strlen(token)+1, sizeof(char));
      if( ! (*argv)[i]){
	perror( "calloc: ");
	exit(EXIT_FAILURE);
      }
      
      strcpy( (*argv)[i], token);
      
      token=strtok(NULL," ");
    } // end if

  free(copy);
  
  return argc;
}/* end makeargs */

//-----------------------------------------------------------------------------
void printargs(int argc, char **argv)
{
  int x;
  for(x = 0; x < argc; x++)
    printf("%s\n", argv[x]);

}/* end printargs */

//-----------------------------------------------------------------------------
/*
  This returns >0 if the word is valid and < 0 if the word is not. 
*/
int isValidWord(char * w )
{
  if(  w == NULL )
    return -1;

  int result = -1;
  int i=0; 

  for( i=0; w[i] != '\0'; i++)
    {
      result = isValidWordChar(w[i]);
      if( result < 0 )
	break;
    }
    

  return result;
} /* end isValidWord */

//-----------------------------------------------------------------------------
/* This functions 1 for valid and < 0 for invalid. 
Valid word characters are as follows: 
A-Z a-z 0-9 - . / ! _
*/
int isValidWordChar(char c )
{
  if( c >= 'a' && c <= 'z' || 
      c >= 'A' && c <= 'Z' ||
      c >= '0' && c <= '9' || 
      c == '-' || c == '.' ||
      c == '/' || c == '!' ||
      c == '_'  )
    return 1;

  return -1;
} /* end isValidWordChar */

//-----------------------------------------------------------------------------
/* This function returns 1 for a valid operator and <0 for an invalid one.
Valid operators are as follows: 
! < > |
*/
int isValidOperator(char c )
{
  if( c == '!' || c == '<' ||
      c == '>' || c == '|' )
    return 1;

  return -1;
} /* end isValidOperator*/

//-----------------------------------------------------------------------------
int getRedirIndex(char * s )
{
  int i=0;
  
  for( i=0; i< strlen(s); i++)
    if( isValidRedirect(&s[i]) > 0 )
	return i;
    
  return -1;
} /* end getRedirIndex*/

//-----------------------------------------------------------------------------
int isValidRedirect(char * r )
{
  if(  r == NULL )
    return -1;

  int result = -1;
  int i=0; 

  result = isValidRedirectChar(r[0]);
  if( result > 0 && r[1] == '\0' ) /* < or > thats it.*/
    return result;
    

  return result;
} /* end isRedirect*/

//-----------------------------------------------------------------------------
int isValidRedirectChar(char c )
{
  if( c=='<' || c == '>')
    return 1;

  return -1;
} /* end isValidRedirectChar */

//------------------------------------------------------------------------------
void strip(char *s)
{
  int len;
  len = strlen(s);
 
  if( len >= 2 )
    {
      if(s[len - 2] == '\r')
	s[len -2] = '\0';

      else if(s[len - 1] == '\n')
	s[len -1] = '\0';
    } // end if
}/* end strip */

//-----------------------------------------------------------------------------
void freeArg(char ** arg, int* len )
{
  int i=0;
  if( *len > 0 )
    {
      for( i=0; i<*len;i++)
	free(arg[i]);
      free(arg);
      *len=0;
    }
}
