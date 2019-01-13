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
    printf("%s\t", builtinStr[i]);
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
  int status = 0;

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
  // Read the line from stdin
  getline(&line, &buffer, stdin);
  return line;
}

// TODO: TEST ME WHEN POSSIBLE
int executeMultipleCommands(char* args){

  int length = strlen(args);
  char* command = malloc(length);

  if(command == NULL){
    fprintf(stderr, "BabyShell: Error while trying to allocate memory.\n");
    exit(EXIT_ERROR);
  }

  int index = 0;
  int status = 1;

  for(int i = 0; i < length; i++ , index++){
    if(args[i] == ';' || i == length - 1){

      command[index] = '\0';
      status = shellExecute(splitShellLine(command));

      free(command);

      if(status != SUCCES_FLAG){
        return status;
      }

      index = -1;
      if(i != length - 1){
          command = malloc(length);
          if(command == NULL){
            fprintf(stderr, "BabyShell: Error while trying to allocate tokenArray \n");
            exit(EXIT_ERROR);
          }
      }
      continue;
    }
    command[index] = args[i];
  }
  return SUCCES_FLAG;
}



void shellLoop(){
  char *readLine;
  char **args;
  int status = 1;

  do {
    printf("> ");
    readLine = readShellLine();

    // If in the read line there is a ; we parse it as multiple commands
    if(strchr(readLine,';') != NULL){
      status = executeMultipleCommands(readLine);
    }else{
      args = splitShellLine(readLine);
      status = shellExecute(args);
      free(args);
    }

    free(readLine);
  } while(status);
}

int main(int argc, char const *argv[]) {

  // Loop used to always await input from user and execute multiple commands
  shellLoop();

  return SUCCES_FLAG;
}
