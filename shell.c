//
// Created by bzane on 2/10/2019.
//
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <wait.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include "shell.h"
#include "command.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int NUM_TOKENS = 0;

void enter_shell() {
    //Update WORKING_DIR

    if (GET_WINDOWS_HOMEPATH) {
        chdir(getenv("HOMEPATH"));
    } else if (GET_UNIX_HOMEPATH) {
        chdir("~");
    }

    char buf[1024];
    char pathbuf[256];
    char **tokenized_input = (char **) malloc(sizeof(char *) * 16);
    for (int i = 0; i < 16; i++) {
        tokenized_input[0] = (char *) malloc(sizeof(char) * 64);
    }

    while (1) {
        bool HAS_REDIRECT = false;
        char* redirect;

        db_printf("enter_shell 1");
        getcwd(pathbuf, sizeof(pathbuf));
        printf("%s@%s> ", pathbuf, getlogin());
        db_printf("enter_shell 2");
        gets(buf);
        db_printf("enter_shell 3");
        parse_cl(buf, tokenized_input, &HAS_REDIRECT, &redirect);


        db_printf("enter_shell 4");
        execcmd(tokenized_input, HAS_REDIRECT, redirect);
    }
}

#pragma clang diagnostic pop

//BEGIN DEVIN KNUD-SECTION
void parse_cl(char input[], char** ret, bool* has_redirect, char** redir_ret) {
    unsigned int tok_index = 0;
    char *token;
    char *remainder = input;

    while ((token = strtok_r(remainder, " ", &remainder))) {
        ret[tok_index] = token;
        if (is_redirect(token)) {
            (*has_redirect) = true;
            (*redir_ret) = token;
        }
        tok_index++;
    }

    NUM_TOKENS = tok_index;
}

void execcmd(char **command, const bool has_redirect, const char* redirector) {
    //Relatively complex macro.
    //Very legal and very cool.
    //All you need to know is that this expands to a macro
    //that individually checks all 3 structs in command.h
    //to see if command[0] matches any of their cmd_str's.
    //If it does, it runs that command and then returns early.
    chk_builtins();

    db_printf("Execcmd 2");

    if (fork() == 0) {
        if (has_redirect) {
            //TODO the redirect operator and the filename make it to the execv call
            //TODO implement 2> support which is literally like 1 change dingus just do it before you commit
            db_printf("Has redirect")
            unsigned int ordinal = 0;
            while (strcmp(">>", command[ordinal++]) != 0);


            char* filename;
            if (command[ordinal]) {
                size_t filename_len = strlen(command[ordinal]);
                filename = (char*) malloc(sizeof(char) * filename_len);
                strcpy(filename, command[ordinal]);

                int file_desc = open(filename, O_CREAT | O_WRONLY, 0666);
                dup2(file_desc, STDOUT_FILENO);
            }
        }

        db_printf("Execcmd 2a");
        char *cmd[NUM_TOKENS];
        cmd[NUM_TOKENS] = NULL;
        for (int i = 0; i < NUM_TOKENS; i++) {
            cmd[i] = command[i];
        }

        db_printf("Execcmd 2b");

        if (execvp(command[0], cmd) == -1) {
            perror("Execution Error");
            exit(EXIT_FAILURE);
        }

        db_printf("Execcmd 2c");
    } else {
        db_printf("Execcmd 3");
        // wait for the child process to end
        int status;
        int exStat = 0;
        if (waitpid(-1, &status, 0) == -1) {
            // if the child process terminates incorrectly, exit status: 1
            exStat = WEXITSTATUS(status);
        } else {
            // if the child process terminates correctly, exit status: 0
            exStat = WEXITSTATUS(status);
        }

        if (DEBUG)
            printf("Exit Status: %d\n", exStat);
        db_printf("Execcmd 3a");
    }
}
