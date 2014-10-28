/****************************
 * Colton Prettyman         *
 * Lab3 OpSys 340 EWU       *
 * Professor Stuart Steiner *
 * 04-22-14                 *
 ****************************/

#include "ssh.h"

/* There is a commented out piece of code in getcwd. This piece of code extracts
   the username from the system in a safe manner. However, there is a bug in 
   this C library call and it appears to leak 60bytes in 1 block. This is a common
   problem and found all over when searching online. For valgrinds sake this
   is commented out. However, if the user so desires this can be easily undone 
*/

//=============================================================================
/* This global variable will store the executing directory*/
char * g_exeDir=NULL;

//-----------------------------------------------------------------------------
/*
  This function is the entry point to the Simple Shell. 
*/
int runssh()
{
  Node * historyList = NULL;
  char input[MAX];
  char * done = NULL;
  char cwd[MAXTEMP];
  char temp[MAXTEMP];
  int runExternal=1;
  int isvalidcmd=-1;
  
  /* Store the executing directory for future reference */
  g_exeDir = calloc(1,sizeof(char)*MAXTEMP);  
  if( ! g_exeDir ){
    perror("calloc: "  );
    exit(EXIT_FAILURE);
  }
  g_exeDir = getcwd(g_exeDir, MAXTEMP);
  
  // changeDirectory(NULL,0,1); 
  initHistory(&historyList);
 
  while(1)
    { 
      /* get the current working directory*/
      getCWD(cwd); 

      /* Get input from the command line */
      grabNextCommand(cwd, input, stdin );
   
      if( strcmp(input, "exit") == 0 )
	{
	  addHistory(input, &historyList);
	  break;
	}
      /* Try running the input command as an internal command */
      runExternal = runAsLocal(input, &historyList);

      /* Just incase the user entered !! and that was exit in history */
      if( strcmp(input, "exit") == 0 )
	{
	  addHistory(input, &historyList);
	  break;
	}

      /* Try running the input command as an external command */
      if( runExternal > 0 ) 
	runAsExternal(input, &historyList);
    }

  free(g_exeDir);
  clearList(&historyList);
  return 1;
}/*end displayMenu*/

//------------------------------------------------------------------------------
/* This grabs the next correct input command of <MAX chars */
void grabNextCommand( char * cwd,  char * input, FILE * fin )
{
  input[0] = '\0';
  char s[MAXTEMP];
  int length=0;
  int i;

  /* loop until valid input is received*/
  while( input[0] == '\0' )
    {
      length = 0;
      printf("%s",cwd);
      if( fgets(s, MAXTEMP, fin ) ) 
	{
	  /* Strip leading whitespaces */
	  while( s[0] == ' ' )
	    strcpy(s, s+1);	
	  
	  /* trim any trailing whitespaces */
	  for(i=strlen(s)-2; i >= 0 && s[i] == ' ' ; i-- )
	    s[i] = '\0';
	    	  
	  /* Count the number of chars before a newline or EOF */
	  for( length=0; length < MAX-1; length++)
	    if( s[length] == '\n' || s[length] == '\0' )
	      break;
	      
	} /* end if */
      
      /* If the breakpoint from above is withing specs copy it to the input string 
	 and return */
      if( length < MAX-1 )
	{
	  strcpy(input, s );
	  input[length]='\0';
	}
      else
	fprintf(stderr, "Invalid input!\n");
    }
} /* end grabNextCommand */

//------------------------------------------------------------------------------
/* This function takes the input and tries to parse it as a locally run command.
   If this is successful it will then decide if it should stay in this local function
   or pull a command from the history and return and run this command externally. 
   This function  will copy the command pulled from history into the input pointer. 
   A return value of > 0 means the input command must be run externally also. A return 
   value < 0 means the command should NOT be run externally 
*/
int runAsLocal(char * input, Node ** historyList )
{
  int staylocal = 1;
  int runExternal = 1;
  char ** localcmd = NULL;
  int localLength = 0;

  while( staylocal == 1 ) 
    {
      staylocal = -1;
      /* Check to see if this is a locally run command */
      if( (localLength = splitLocalCommand(input, &localcmd )) > 0 )
	{
	  if( localcmd == NULL )
	    {
	      /* why is this null?*/
	      fprintf(stderr,"Empty command\n");
	      exit(EXIT_FAILURE);
	    }
	 
	  if( strcmp(localcmd[0], "cd" ) == 0 ) /* Check for cd */
	    {
	      changeDirectory(localcmd, localLength, -1 );
	      runExternal=-1;
	      staylocal=-1;
	      addHistory(input, historyList);
	    }
	  else if( strcmp(localcmd[0], "history") == 0 ) /* check for history */
	    {
	      addHistory(input, historyList);
	      printList(*historyList);
	      runExternal=-1;
	      staylocal=-1;
	    }
	  else if(strncmp(localcmd[0], "!", strlen("!") ) == 0 ) /* check for !# */
	    {
	      int index=0;
	      Node * n = NULL;
	      Word * w = NULL;
	      staylocal=1;
	      runExternal=1;

	      if( isdigit((localcmd[0])[1]) > 0  )
		index = atoi(localcmd[0]+1);
	      else if( strcmp(localcmd[0], "!!")  == 0 )
		index = getListSize(); /* get last command */
	      else
		break;
	      
	      if(index <= 0 )
		{
		  fprintf(stderr,"Simple Shell: %s: event not found\n", localcmd[0]);
		  runExternal = -1;
		  staylocal = -1;
		  break;
		}

	      n = getElement(*historyList, index);

	      if( n != NULL )
		{
		  w = n->data;
		  strcpy(input, w->word );
		  printf("%s\n",input);
		  runExternal=1;
		}
	      else
		{
		  fprintf(stderr,"Simple Shell: %s: event not found\n", localcmd[0]);
		  runExternal = -1;
		  staylocal = -1;
		}
	    }
	  else
	    fprintf(stderr,"%s: command not found\n", input);
	  
	  freeArg(localcmd, &localLength);
	} /* end if */
    }/* end while */

  return runExternal;
} /* end runAsLocal */

//------------------------------------------------------------------------------
/* This function takes the input and attempts to parse and run it as an external
   command. This means it will attempt to fork, pipe, redirect, and exec as needed.
*/
int runAsExternal(char * input, Node ** historyList )
{
  addHistory(input, historyList);
  cmd ** cmds=NULL;
  int numcmds=0;

  numcmds = splitExternalCommands(&cmds, input);
  int numpipes=0;
  int curpipe=0;
  int status; 
  
  if( numcmds > 1 )
    numpipes = numcmds -1;

  if( numcmds > 0 )
    {
      int i=0;
      int pipefds[2*numpipes];

      /* Initialize the pipe file descriptor array */
      for(i=0;i< numpipes; i++)
	if( pipe(pipefds + i*2) < 0 ){
	  perror("pipe: ");
	  exit(EXIT_FAILURE);
	}

      /* Loop through executing each command */
      int curcmd=0;
      for( curcmd=0, curpipe=0; curcmd < numcmds; curcmd++, curpipe+=2)
	{ /* iterate one at a time running each comand */
	  cmd * cmd = cmds[curcmd];
	  pid_t pid;
	  FILE ** redir = NULL;
	  int i=0;

	  /****************File redirection ********************************/
	  /* Create the array of file redirects */
	  if( cmd->redirlength > 1 ) 
	    {
	      // int ouput=1;
	      redir = malloc(sizeof(FILE*)*(cmd->redirlength/2));
	      if( ! redir ){
		perror("malloc: ");
		exit(EXIT_FAILURE);
	      }
	  
	      /* Redirect and open all file */
	      for(i=0;i<cmd->redirlength; i+=2)
		{
		  /* Try opening existing file */
		  redir[i/2] = fopen( cmd->redirects[i+1], "r+");
	  
		  /* No file...create one */
		  if(!redir[i/2]) 
		    redir[i/2] = fopen( cmd->redirects[i+1], "w+");
	  
		  /* Neither worked */
		  if( !redir[i/2]){
		    perror("fopen: ");
		    return -1;
		  } 
		} /* end for*/
	    }/* end if */
  
	  /**************Fork and exec the command ***********************/
	  pid = fork();

	  if( pid == 0 ) /* Child */
	    {
	      /*FileRedirection*/
	      if( cmd->redirlength > 1 && redir != NULL) 
		{
		  /* Redirect and open all file */
		  for(i=0;i<cmd->redirlength; i+=2)
		    {
		      /* Pipe stdout to "foo.txt" */
		      if( strcmp( cmd->redirects[i], ">") == 0 )
			dup2(fileno(redir[i/2]),STDOUT_FILENO);
		      else  /* Pipe "foo.txt" into stdin */
			dup2(fileno(redir[1/2]),STDIN_FILENO);
		    } /* end for*/
		}/* end if */
      
	      /*************Set up pipes *****************/
	      /* Not the last command */
	      if( numcmds != curcmd +1 && redir == NULL){
		if(dup2(pipefds[curpipe+1], 1) < 0){
		  perror("dup2: ");
		  exit(EXIT_FAILURE);
		}
	      } /* end if */

	      /*if not first command&& j!= 2*numPipes*/
	      if(curcmd != 0 && redir == NULL ){
		if(dup2(pipefds[curpipe-2 ], 0) < 0){
		  perror("dup2: ");
		  exit(EXIT_FAILURE);
		}
	      } /* end if */
	      
	      /* Close the pipes */
	      for(i = 0; i < 2*numpipes && redir == NULL; i++){
		close(pipefds[i]);
	      }
	    
	      /* exec the program */
	      if( execvp(cmd->command[0], cmd->command) == -1 )
		if( cmd !=NULL){ /* The command failed */
		  perror(cmd->command[0]);
		  exit(EXIT_FAILURE);
		}
	    } /* end if */
	  else if( pid < 0 ) /* fork error */
	    {
	      perror("fork: ");
	      exit(EXIT_FAILURE);
	    }

	  /* Back in parent */
	  /* Close the redirection files */
	  if( cmd->redirlength > 1 && redir != NULL)
	    {
	      for(i=0;i < cmd->redirlength/2; i++)
		fclose(redir[i]);
	      free(redir);
	    } 
  
	  
	  //runCommand(cmds[i], numcmds, curcmd, pipefds, numpipes, curpipe);
	} /* end for */
      
      /* In parent */
      /* Close all open pipes */
      for(i = 0; i < 2 * numpipes; i++){
        close(pipefds[i]);
      }
      
      /* Wait for all children to finish */
      for(i = 0; i < numcmds; i++)
        wait(&status);

      // printcmds(cmds, numcmds);
      cleancmds(cmds, &numcmds);
      free(cmds);
    }
  else
    {
      fprintf(stderr, "Invalid Command!\n");
      return -1;
    }
  
 
  return 1;
} /* end runAsExternal */

//------------------------------------------------------------------------------
//int runCommand( cmd * cmd , int numcmds, int curcmd, int * pipefds, int numpipes, int curpipe)
//{
  /* 
     This would be a function if filedescriptor arrays could be passed around
     without them being messed with in the new function stack frame. 
     Hence the very large functino above 
  */
//} /* end run command */

//------------------------------------------------------------------------------
/* gets and returns the current working directory, NULL on error */
char * getCWD(char * cwd )
{
  char temp[MAXTEMP];
  /* uid_t uid = geteuid();
  struct passwd * pw = getpwuid(uid);
  
  if(pw != 0)
    {
      strcpy(cwd, pw->pw_name);
    }
    else*/
  strcpy( cwd, "user");
  //getlogin(temp, MAXTEMP);


  strcat(cwd,"@");
  if( gethostname(temp, MAXTEMP) == -1 )
    perror("gethostname: ");
  strcat(cwd, temp );
  strcat(cwd, ":~");
    
  if(! getcwd(temp, MAXTEMP ) ){
    perror("getcwd: ");
    exit(EXIT_FAILURE);
  }
  strcat(cwd,temp);
  strcat(cwd,"$ ");

  return cwd;
}/* end getCWD */

//------------------------------------------------------------------------------
int changeDirectory(char ** newdir, int len ,  int reset )
{
  /* This resets it to the starting directory */
  if( reset == 1 || len == 1 || newdir == NULL)
    { 
      if( chdir(g_exeDir ) != 0 ) { //getenv("CWD")); 
	perror("chdir: " );
	return -1;
      }
    } /* may need to user getcwd here, HOME */
  else
    {
      if( chdir( newdir[1] ) != 0) {
	perror("chdir: " );
	return -1;
      }
    }
  
  return 1;
} /* end changeDirectory*/

//------------------------------------------------------------------------------
Node * initHistory(Node ** history )
{
  char temp[MAX];
  Node * newNode=NULL;
  char filename[MAXTEMP];
  strcpy(filename, g_exeDir);
  strcat(filename,"/");
  strcat(filename, HISTFILE);

  FILE * fin  = fopen(filename, "r");

  /* No file...create an emtpy one */
  if( ! fin ) {
    fin = fopen(filename, "w");
    if( !fin )
      {
	perror("fopen: ");
	exit(EXIT_FAILURE);
      }
    fclose(fin);
    return * history;
  }

  /* Read in the whole file and put it in a linkedList*/
  while( fgets(temp, MAX, fin) )
    {
      strip(temp);
      newNode = createNode(temp);
      addLast(newNode, history);
      newNode = NULL;
    }
  
  fclose(fin);
 
  return *history;
} /* end initHistory */

//------------------------------------------------------------------------------
void addHistory(char * s, Node ** history )
{
  Node * newNode = getLast(*history);
  if( newNode != NULL )
    {
      Word * w = newNode->data;
      
      /* the last command entered was the one just entered */
      if( strcmp(w->word, s ) == 0 )
	return;
    }

  /* this is a fresh new command */
  newNode = createNode(s);
  addLast(newNode, history);
  //  saveHistory(*history);
  saveCommandToHistory( s );
} /* end addHistory*/

//------------------------------------------------------------------------------
void saveHistory(Node * history )
{
  char filename[MAXTEMP];
  strcpy(filename, g_exeDir);
  strcat(filename,"/");
  strcat(filename, HISTFILE);

  FILE * fout = fopen(filename, "w");
  
  if( !fout ) {
    perror("fopen: ");
    exit(EXIT_FAILURE);
  }
  
  fprintList(history, fout );
  fclose(fout);
} /* end saveHistory*/

//------------------------------------------------------------------------------
void saveCommandToHistory(char * command )
{
  char filename[MAXTEMP];
  strcpy(filename, g_exeDir);
  strcat(filename,"/");
  strcat(filename, HISTFILE);

  FILE * fout = fopen(filename, "a+");
  
  if( !fout ) {
    perror("fopen: ");
    exit(EXIT_FAILURE);
  }
  
  fprintf(fout, "%s\n", command );
  //fprintList(history, fout );
  fclose(fout);
} /* end saveHistory*/

//------------------------------------------------------------------------------
Node * createNode(char * buffer )
{
  Node * temp = NULL;
  Word * word = NULL;
  int length=0;
  
  length = strlen(buffer); 

  word = calloc(1,sizeof(Word));
  if( ! word )
    {
      fprintf(stderr, "Error allocating word in createNode!\n");
      exit(EXIT_FAILURE);
    }
  
  word->length = length;
  
  word->word = calloc(1,length*sizeof(char) + 1 ); // +1 for '\0'
  if( ! word->word )
    {
      fprintf(stderr, "Error allocating word data in createNode!\n");
      exit(EXIT_FAILURE);
    }

  strcpy(word->word, buffer);
  
  temp = calloc(1, sizeof(Node));
  if( ! temp )
    {
      fprintf(stderr, "Error allocating Node in createNode!\n");
      exit(EXIT_FAILURE);
    }
  
  temp->data = word;
  
  return temp;
}/* end createNode*/

