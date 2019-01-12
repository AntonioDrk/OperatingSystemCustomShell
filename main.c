#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define SUCCES_FLAG 0
#define EXIT_ERROR -1
#define MAXLENGTH_TOK_ARRAY 32
#define TOKEN_DELIMITATORS " \t\a\n\r"

// Splits the arguments into a vector and returns it
char ** splitShellLine(char* line){
  // The length of the token array, this will be incremented
  // dynamically, if it needs to
  int maxLength = MAXLENGTH_TOK_ARRAY;
  int index = 0;

  char ** tokenArray = malloc(maxLength * sizeof(char*));
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
  tokenArray[position] = NULL;
  return tokenArray;
}

char* readShellLine(){
  char *line = NULL;
  int buffer = 0;
  // Citeste linia din stdin
  getline(&line, &buffer, stdin);
  return line;
}

void shellLoop(){
  char *readLine;
  char *args[];
  int status;

  do {
    readLine = readShellLine();
    args = splitShellLine(readLine);
    // execute command with args

    free(readLine);
    free(args);
  } while(stauts);
}

int int main(int argc, char const *argv[]) {

  // Loop used to always await input from user and execute multiple commands
  shellLoop();

  return SUCCES_FLAG;
}
