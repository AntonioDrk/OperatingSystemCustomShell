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
