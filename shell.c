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
#include <string.h>
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
        redirec_t redirects[E_REDIRECT_N];
        char** filenames = (char**) malloc(sizeof(char*) * E_REDIRECT_N);
        for (int i = 0; i < E_REDIRECT_N; i++) {
            filenames[i] = (char*) malloc(sizeof(char) * 64);
        }

        db_printf("enter_shell 1");
        getcwd(pathbuf, sizeof(pathbuf));
        printf("%s$ ", pathbuf);
        db_printf("enter_shell 2");
        gets(buf);
        db_printf("enter_shell 3");
        if (parse_cl(buf, tokenized_input, &HAS_REDIRECT, redirects, filenames) == 0) {
            db_printf("enter_shell 4");
            if (DEBUG) {
                for (int i = 0; i < E_REDIRECT_N; i++) {
                    printf("redirects[%d]=%s, filename=%s\n", i, redirect_to_str(redirects[i]), filenames[i]);
                }
                continue;
            }

            execcmd(tokenized_input, HAS_REDIRECT, redirects);
        } else {
            perror("Parse error\n");
        }

        free(filenames);
    }
}

#pragma clang diagnostic pop

//BEGIN DEVIN KNUD-SECTION
int parse_cl(char input[], char** ret, bool* has_redirect, redirec_t redirects[E_REDIRECT_N], char** filenames) {
    unsigned int tok_index = 0;
    char *token;
    char *remainder = input;
    unsigned int num_redirects = 0, num_filenames = 0;
    bool processing_redirects = false;

    while ((token = strtok_r(remainder, " ", &remainder))) {

        if (is_redirect(token)) {
            (*has_redirect) = true;
            processing_redirects = true;
            redirec_t redirect = str_to_redirect(token);

            for (int i = 0; i < E_REDIRECT_N; i++) {
                if (redirects[i] == redirect) {
                    perror("Redundant redirects");
                    return -1;
                }
            }

            redirects[num_redirects++] = redirect;
            continue;
        } else if (!(is_redirect(token)) && processing_redirects) {
            filenames[num_filenames++] = token;
            continue;
        }

        ret[tok_index++] = token;
    }

    if (num_filenames != num_redirects) {
        if (DEBUG) {
            printf("num_redirects=%d, num_filenames=%d\n", num_redirects, num_filenames);
        }
        perror("Redirect syntax error");
        return -1;
    }

    NUM_TOKENS = tok_index;
    return 0;
}

void execcmd(char **command, const bool has_redirect, const redirec_t redirector[]) {
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

redirec_t str_to_redirect(char* redirect) {
    if (cmp_redir(redirect, ">>")) {
        return STDOUT;
    } else if (cmp_redir(redirect, "<<")) {
        return STDIN;
    } else if (cmp_redir(redirect, "2>")) {
        return STDERR;
    }
}

char* redirect_to_str(redirec_t redirect) {
    switch (redirect) {
        case STDIN:
            return "<<";
        case STDOUT:
            return ">>";
        case STDERR:
            return "2>";
        default:
            return NULL;
    }
}
