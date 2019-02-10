//
// Created by bzane on 2/10/2019.
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include "command.h"
#include "shell.h"

/**
 * The Mac Daddy function that parses input from the user.
 * Returns:
 * -3 if the command was found but an incorrect number of arguments were passed.
 * -2 if the command was found and executed but an error occurred.
 * -1 if the command wasn't found.
 * 0 if nothing went wrong.
 */
int parse(char* args[]) {
    for (int i = 0; i < NUM_COMMANDS; i++) {
        cmd_t* command = ALL_COMMANDS[i];

        if (strcmp(command->cmd_str, args[0]) == 0) {
            (command->function)(args); //TODO don't disregard the return status
            return 0;
        }
    }

    return -1;
}

/**
 * Initializes all structs contained in command.h.
 */
void init_cmd_structs() {
    //Init for "cd" command
    cd = malloc(sizeof(cmd_t));
    cd->cmd_str = "cd";
    cd->function = _builtin_cd;
    cd->usage_message = "cd <dir>";

    //Init for "pwd" command
    p_wd = malloc(sizeof(cmd_t));
    p_wd->cmd_str = "pwd";
    p_wd->function = _builtin_pwd;
    p_wd->usage_message = "pwd (no args)";

    //Init for "exit" command
    exit_cmd = malloc(sizeof(cmd_t));
    exit_cmd->cmd_str = "exit";
    exit_cmd->function = _builtin_exit;
    exit_cmd->usage_message = "exit (no args)";
    
    ALL_COMMANDS[0] = cd;
    ALL_COMMANDS[1] = p_wd;
    ALL_COMMANDS[2] = exit_cmd;
}

/**
 * Builtin command for changing the working directory.
 * Calls chdir in unistd.h and updates WORKING_DIR in shell.h
 * @param dir
 * @return
 */
int _builtin_cd(char* args[]) {
    char* dir = args[0]; //TODO I dont think this is the correct index
    WORKING_DIR = dir;
    return chdir(dir);
}

/**
 * Builtin command for printing the current working directory.
 * @return
 */
int _builtin_pwd(char* args[]) {
    printf("%s\n", WORKING_DIR);
    return 0;
}

/**
 * Builtin command for exiting.
 * @return
 */
int _builtin_exit(char* args[]) {
    exit(0);
}

