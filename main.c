#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define SUCCES_FLAG 1
#define ERROR_FLAG -1
#define EXIT_ERROR -1337
#define MAXLENGTH_TOK_ARRAY 32
#define TOKEN_DELIMITATORS " \t\a\n\r"

// Function Declarations for the builtin shell commands
int shellHelp(char **args);
int shellExit(char **args);
int shellCd(char **args);

// List of builtin commands
char *builtinStr[] = {
  "help",
  "exit",
  "cd"
};

// IMPORTANT: MAKE SURE THAT THEY ARE IN THE SAME POSITION IN BOTH ARRAYS!!!

// List of pointers to the functions
int (*builtinFunc[]) (char **) = {
  &shellHelp,
  &shellExit,
  &shellCd
};

// Get the length of the commands array ()
int shellNumBuiltin(){
  return sizeof(builtinStr) / sizeof(char *);
}

// IMPLEMENTATION FOR BUILTIN COMMANDS
int shellHelp(char **args){
  printf("BabyShell 0.1 \n");
  printf("This shell was made by: George, Ema, Antonio as a project for Operating Systems class.\n");
  printf("Type a programs name and it's arguments into the terminal and hit enter\n");
  printf("The following commands are built in:\n");
  for(int i = 0; i < shellNumBuiltin(); i++){
    printf("\t%s", builtinStr[i]);
  }

  printf("\nUse the man command for information about other programs\n");

  return SUCCES_FLAG;
}

// Returns status 0 so the shellLoop will terminate
int shellExit(char **args){
  return 0;
}

int shellCd(char **args){
  if(args[1] == NULL){
    fprintf(stderr, "BabyShell: no argument for \"cd\" found\n");
  } else{
    if(chdir(args[1]) != 0){
      perror("BabyShell");
    }
  }
  return SUCCES_FLAG;
}


// Launch PROGRAM from shell
int shellLaunch(char** args){
  pid_t pid;
  int status;

  pid = fork();
  if(pid == 0){
    // Child process
    if(execvp(args[0],args) == -1){
      perror("BabyShell");
    }
    exit(EXIT_ERROR);
  }else if (pid < 0){
    // Error while trying to fork
    perror("BabyShell");
  }else{
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return SUCCES_FLAG;
}


// Tries to execute first a builtin command, if none is found then
// Tries to launch a program from shell
int shellExecute(char **args){
  if(args[0] == NULL){
    // Empty command (just an enter), continue reading from input
    return SUCCES_FLAG;
  }

  for(int i = 0; i < shellNumBuiltin(); i++){
    if(strcmp(args[0],builtinStr[i]) == 0){
      // It's a builtin command
      return (*builtinFunc[i]) (args);
    }
  }

  // If it isn't a built in function try to launch a program
  return shellLaunch(args);
}

int spawn_proc (int in, int out, char **args)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (args[0], args);
    }

  return pid;
}

int fork_pipes (int n, ***commands)
{
  int i;
  pid_t pid;
  int in, fd [2];

  /* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);

      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], commands[i]);

      /* No need for the write end of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */  
  if (in != 0)
    dup2 (in, 0);

  /* Execute the last stage with the current process. */
  return execvp (commands[n-1][0], commands[n-1]);
}

int shellPipe(char **args) {
	char ***commands;
	int n = 0;
	char **token = strtok(args,"|");
	while(token!=NULL) {
		commands = realloc(commands, (n+1)*sizeof(char**));
		strcat(commands[n], token);
		n++;
		
		token=strtok(args,"|");
	}
	fork_pipes(n, commands);
}

// Splits the arguments into a vector and returns it
char ** splitShellLine(char* line){
  // The length of the token array, this will be incremented
  // dynamically, if it needs to
  int maxLength = MAXLENGTH_TOK_ARRAY;
  int index = 0;

  char **tokenArray = malloc(maxLength * sizeof(char*));
  char *token;

  if(tokenArray == NULL){
    fprintf(stderr, "BabyShell: Error while trying to allocate tokenArray \n");
    exit(EXIT_ERROR);
  }

  token = strtok(line,TOKEN_DELIMITATORS);
  while(token != NULL){
    tokenArray[index] = token;
    index++;

    if(index >= maxLength){
      maxLength += MAXLENGTH_TOK_ARRAY;
      tokenArray = realloc(tokenArray, maxLength * sizeof(char*));

      if(tokenArray == NULL){
        fprintf(stderr, "BabyShell: Error while trying to allocate tokenArray \n");
        exit(EXIT_ERROR);
      }
    }

    token = strtok(NULL,TOKEN_DELIMITATORS);
  }
  tokenArray[index] = NULL;
  return tokenArray;
}

char* readShellLine(){
  char *line = NULL;
  size_t buffer = 0;
  // Citeste linia din stdin
  getline(&line, &buffer, stdin);
  return line;
}

// TODO: TEST ME WHEN POSSIBLE
int executeMultipleCommands(char** args){
	char** command;
  char* token = args[0];
	int i, argsCounter = 0;
	int lengthTokens = 1, semiColIndex = 0;

  // Getting the length of the args array + the index of the semicolon
  while( token != NULL ){
    if(strcmp(args[lengthTokens], ";") == 0){
      semiColIndex = lengthTokens;
    }
    lengthTokens++;
  }
  // We allocate first the size of the first command + it's arguments
  // Till the index of ;
  command = malloc(sizeof(char*) * semiColIndex);
	for(i = 0; i < lengthTokens; i++)
		if(strcmp(args[i],";") == 0)
		{
			strcpy(command[argsCounter], args[i]);
  		argsCounter++;
		}
		else if(strcmp(args[i],";") == 0 || i == lengthTokens - 1)
		{
			shellExecute(command);
			free(command);

      // Allocation of the remaining memory after the semicolon
			command = malloc(sizeof(char*) * (lengthTokens - semiColIndex - 1) );
			argsCounter = 0;
		}
}



void shellLoop(){
  char *readLine;
  char **args;
  int status;

  do {
    printf("> ");
    readLine = readShellLine();
    args = splitShellLine(readLine);
    status = shellExecute(args);

    free(readLine);
    free(args);
  } while(status);
}

int main(int argc, char const *argv[]) {

  // Loop used to always await input from user and execute multiple commands
  shellLoop();

  return SUCCES_FLAG;
}
