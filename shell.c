//
// Created by bzane on 2/10/2019.
//
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include "shell.h"
#include "command.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void enter_shell() {
    //Update WORKING_DIR

    if (GET_WINDOWS_HOMEPATH) {
        WORKING_DIR = getenv("HOMEPATH");
    } else if (GET_UNIX_HOMEPATH) {

    }

    chdir(WORKING_DIR);

    char buf[1024];
    char* tokenized_arguments[16];
    unsigned int token_index = 0;

    while (1) {
        printf("%s@%s> ", WORKING_DIR, getlogin());
        gets(buf);

        char* token;
        char* remainder = buf;

        while ((token = strtok_r(remainder, " ", &remainder))) {
            tokenized_arguments[token_index] = token;
            token_index++;
        }

        parse(tokenized_arguments);
        token_index = 0;
        memset(tokenized_arguments, 0, sizeof(tokenized_arguments));
    }
}
#pragma clang diagnostic pop

int process_external_command(char* args[]) {

}

