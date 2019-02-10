//
// Created by bzane on 2/10/2019.
//

#include <stdio.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void enter_shell() {
    chdir(WORKING_DIR);

    while (1) {
        printf("%s@%s> ", WORKING_DIR, getlogin());
        (exit_cmd->function)(NULL); //Test struct inits
    }
}
#pragma clang diagnostic pop
