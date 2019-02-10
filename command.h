//
// Created by bzane on 2/10/2019.
//

#ifndef TECHSHELL_COMMAND_H
#define TECHSHELL_COMMAND_H

#define NUM_COMMANDS 3

/**
 * The structure that stores the name of a command
 * and its associated function.
 */

typedef int (*associated_function)();
typedef char* command_string;

typedef struct Command {
    command_string cmd_str;
    associated_function function;
} cmd_t;

static cmd_t* ALL_COMMANDS[NUM_COMMANDS];

cmd_t* cd;
cmd_t* p_wd;
cmd_t* exit_cmd;

void init_cmd_structs(void);

//Built-in commands
int _builtin_cd(char* dir);
int _builtin_pwd();
int _builtin_exit();

#endif //TECHSHELL_COMMAND_H
