#include "./penn-shredder.h"

void sigHandlerAlarm(int sigNum){
    //Print catchphrase and kill child process if timer is reached
    if (isRunning == 1) {
        printf(CATCHPHRASE);
        killResult = kill(0, sigNum);
        checkResult(killResult, "Invalid kill");
        isRunning = 0;
    }
}

void sigHandlerInt(int signum){
    //new-line and re-promt after Control-C
    write(STDERR_FILENO, "\n", 1);
    write(STDERR_FILENO, PROMPT, promptLen);
}

void checkResult(int result, char* errorMessage) {
    //Checks to see if a result was invalid and exit(1) if so
    if (result == -1){
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    while (1) {
        //Signal Handelers
        signal(SIGINT, sigHandlerInt);
        signal(SIGALRM, sigHandlerAlarm);

        //reset variables 
        char cmd[maxLineLength] = "";
        char cmdCopy[maxLineLength] = "";
        isRunning = 0;

        //Get input from user
        write(STDERR_FILENO, PROMPT, promptLen);
        numBytes = read(STDIN_FILENO, cmd, maxLineLength);
        checkResult(numBytes, "Invalid read");
        strcpy(cmdCopy, cmd);

        //Handle cases of Control-D being hit 
        // no '\n' is present in this case at the end of cmd
        // so test for that
        if(numBytes == 0 && !isspace(cmd[numBytes-1])) {
            printf("\n");
            exit(0);
        } else if (!isspace(cmd[numBytes-1])) {
            printf("\n");
            continue;
        }
        //Test to see if they set an optional timer and set the timer is so
        if (setTimer(cmdCopy) == 1) {
            continue;
        }

        //Get arguments from user arguments
        char** arguments = getArgsFromCommand(cmd, numBytes);
        char* const env[] = { NULL };

        //Fork and run commands 
        pid = fork();
        checkResult(pid, "Invalid fork");
        if (pid == 0) {
            childResult = execve(arguments[0], arguments, env);
            checkResult(childResult, "Invalid");
        } else if (pid > 0) {
            isRunning = 1;
            alarm(timer);
            parentResult = wait(NULL);
            checkResult(parentResult, "Invalid wait");
        }
        //Free variables used for the arguments
        for (int x=0; x<numArgs+1; x++) {
            free(arguments[x]);
        }
        //Re-set alarm
        alarm(0);
        //Free pointers 
        free(arguments);
    }
    return 0;
}

int setTimer(char* command) {
    //Checks if timer was given and sets current timer
    //Kills process if more than one argument is given 
    char* number = strtok(command, "    \n");
    if (number == NULL) {
        return 1;
    }
    if (!isdigit(*number)) {
        return 0;
    }
    timer = atoi(number);
    number = strtok(NULL, "    \n");
    if (number != NULL || timer < 0) {
        checkResult(-1, "Invalid arguments");
    }
    return 1;
}

char** getArgsFromCommand(char* commandForArgs, int numBytes) {
    //Recieves commands and parses them
    //Returns a char[][] array of the arguments given 
    char args[numBytes][numBytes];
    numArgs = 0;
    char *search = "    \n";
    char *currArg = strtok(commandForArgs, search);

    while (currArg != NULL) {
        strcpy(args[numArgs], currArg);
        numArgs++;
        currArg = strtok(NULL, search);
    }
    
    char **arguments = malloc((numArgs + 1) * sizeof(char *));
    int i = 0;
    for (i = 0; i < numArgs; i++) {
        arguments[i] = malloc((strlen(args[i]) + 1) * sizeof(char));
        strcpy(arguments[i], args[i]);
    }
    arguments[i] = NULL;
    return arguments;
}

