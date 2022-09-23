#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#define maxLineLength 4096

void readInput();
void checkResult(int, char*);
char** getArgsFromCommand(char*, int);
int setTimer(char*);
const int promptLen = strlen(PROMPT);
char* token;
pid_t  pid;
int timer = 3;
int numArgs = 0;
int numBytes = 0;
int killResult = 0;
int childResult = 0;  
int parentResult = 0; 
int isRunning = 0;