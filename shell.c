#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <signal.h>

int switchparser(char input[256]); //function decleration 
void execute(); //execute function

char cd[256]; //cd argument storage
char argumentlist[15][15]; //execvp argument list 
char* command;
int destsize;


main(int argc, char **argv) 
{
    int i; //for-loop stuff
    int pid, ppid; //pid and ppid command storage
    int bashkill = 0; //to end the bash 
    char headername[30] = "308sh>"; //changing header name for bash
    char inputstring[256]; //input string for bash 
    char cwd[256]; //pwd storage
    char *home; //storage for home path variable

    for(i = 0; i < argc; i++) //for parsing data
    {
        if(strcmp(argv[i], "-p") == 0)
        {
            strcpy(headername, argv[i + 1]);

            break;
        }
    } 
    
    while(bashkill != 1) //while loop for the bash
    {
        printf("%s ", headername);
        fgets(inputstring, 256, stdin);

        switch(switchparser(inputstring))
        {
            case 1: //exit
                bashkill = 1;
                break;
            case 2: //pwd
                getcwd(cwd, sizeof(cwd));
                printf("%s\n", cwd);
                break;
            case 3: //pid
                pid = getpid();
                printf("%d\n", pid);
                break;
            case 4: //ppid
                ppid = getppid();
                printf("%d\n", ppid);
                break;
            case 5: //cd (no arguments)
                home = getenv("HOME");
                chdir(home);
                break;
            case 6: //cd (arguments)
                chdir(cd);
                break;
            case 7: //execvp TODO 
                execute();
                break; 
            default : 
                printf("This command is not found\n");
        }
    }

    return 0;
}

int switchparser(char input[256]) //parse the input for an easier switch statement 
{
    char token[50][256]; //bit of char for info
    char *STATIC; 
    const char s[4] = " \n"; //delimiter
    int switchassignment; //assignment number for the switch statment 
    int i = 0; //token array movement 
    int j = 0; //execvp array movement 
    
    //first input 
    STATIC = strtok(input, s);
    strcpy(token[i], STATIC);

    while(STATIC != NULL) //multi-worded input
    {
        STATIC = strtok(NULL, s);

        if(STATIC != NULL)
        {
            i = i + 1;
            strcpy(token[i], STATIC);
        }
    }

    destsize = i;

    //inputs
    if(strcmp(token[0], "exit") == 0) //exit
    {
        switchassignment = 1;
    }
    else if(strcmp(token[0], "pwd") == 0) //pwd
    {
        switchassignment = 2;
    }
    else if(strcmp(token[0], "pid") == 0) //pid
    {
        switchassignment = 3;
    }
    else if(strcmp(token[0], "ppid") == 0) //ppid
    {
        switchassignment = 4;
    }
    else if(strcmp(token[0], "cd") == 0) //cd 
    {
        if(i < 1) //cd
        {
            switchassignment = 5; 
        }
        else //cd /etc/etc/etc
        {
            strcpy(cd, token[1]);

            switchassignment = 6; 
        } 
    }
    else //execvp 
    {
        command = token[0];

        for(j = 0; j <= i; j++)
        {
            strcpy(argumentlist[j], token[j]);
        }

        switchassignment = 7;
    }
   
    return switchassignment;
}

void execute() //execvp function 
{
    int status, i, j, d = 0, size[14];
    char* destination[destsize + 2];

    for(i = 0; i < destsize + 1; i++) //populating destination with the arguments
    {
        destination[i] = argumentlist[i];
    }

    destination[destsize + 1] = NULL;

    int pid = fork();

    if(pid == 0) //forks into execvp
    {
        int childpid = getpid();

        printf("Child PID: %d\n", childpid);

        int statuscode = execvp(command, destination);

        if(statuscode == -1)
        {
            printf("execvp Terminated Incorrectly\n");
            
            kill(childpid, SIGKILL);
        }
    }
    else //old parent process, program will come here when finished with execvp
    {
        waitpid(-1, &status, 0);
        
        if(WIFEXITED(status)) //exit status printing 
        {
            const int es = WEXITSTATUS(status);

            printf("Child PID | Exit Status:\n [%d]   |   [%d]\n", pid, es);
        }

        printf("Child Process has completed.\n");
    }
}

