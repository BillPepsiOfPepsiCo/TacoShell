#include<stdio.h>
#include<unistd.h>
#include<wait.h>
#include<stdlib.h>
#include<string.h>

//char* str = char str[]
//char * str[] = char** str

int AMNT_TOKS = 0;
int DEBUG = 0;

void parseCommandLine(char input[], char* ret[])
{
        input[strcspn(input, "\n")] = '\0';
        
	if(DEBUG)
		printf("Input = %s\n", input);

	int amntToks = 0;
        char *toks;
        toks = strtok(input, " ");
	while(toks != NULL)
	{
		if(DEBUG)
			printf("toks %i = %s\n", amntToks, toks);

		ret[amntToks] = toks;
		amntToks ++;
		toks = strtok(NULL, " ");
	}
	
	if(DEBUG)
	{
		printf("Amount of Tokens: %i\n", amntToks);

		for(int i = 0; i < amntToks; i++)
		{
			printf("ret[%i] = %s\n", i, ret[i]);
		}
	}

	
	AMNT_TOKS = amntToks;
}

void executeCommand(char* command[])
{
        if(fork() == 0)
        {
		char *cmd[AMNT_TOKS];
                cmd[AMNT_TOKS] = NULL;
                for(int i = 0; i < AMNT_TOKS; i++)
                {
                       cmd[i] = command[i];
                }

                if(execvp(command[0], cmd) == -1)
                {
                       perror("Execution Error");
                        exit(EXIT_FAILURE);
                }
        }
        else
        {
                // wait for the child process to end
                int status;
                int exStat = 0;
                if(waitpid(-1, &status, 0) == -1)
                 {
                         // if the child process terminates incorrectly, exit status: 1
                         exStat = WEXITSTATUS(status);
                 }
                 else
                 {
                         // if the child process terminates correctly, exit status: 0
                         exStat = WEXITSTATUS(status);
                 }
		 
		 if(DEBUG)
			 printf("Exit Status: %d\n", exStat);
        }
}

int main()
{
	int size = 256;
	char inputCom[size];

        printf("$ ");
        fgets(inputCom, size, stdin);

	char* parsedCom[256];
       	parseCommandLine(inputCom, parsedCom);	

	if(DEBUG)
	{
		int i = 0;
		while(parsedCom[i] != NULL)
		{
			printf("parsedCom[%i] = %s\n", i, parsedCom[i]);
			i++;
		}
	}
	
	executeCommand(parsedCom);
	
	return 0;
}
