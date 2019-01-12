#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define SUCCES_FLAG 0

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
    // separate command and args
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
