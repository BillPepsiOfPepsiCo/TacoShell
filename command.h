//
// Created by bzane on 2/10/2019.
//

#ifndef TECHSHELL_COMMAND_H
#define TECHSHELL_COMMAND_H

#define NUM_COMMANDS 3
#define MAX_COMMAND_ARGS 10

/**
 * The structure that stores the name of a command
 * and its associated function.
 */

typedef int (*associated_function)(char*[]);
typedef char* command_string;

typedef struct Command {
    command_string cmd_str;
    associated_function function;
    char* usage_message;
} cmd_t;

static cmd_t* ALL_COMMANDS[NUM_COMMANDS];

cmd_t* cd;
cmd_t* p_wd;
cmd_t* exit_cmd;

void init_cmd_structs(void);
int parse(char*[]);

//Built-in commands
int _builtin_cd(char*[]);
int _builtin_pwd(char*[]);
int _builtin_exit(char*[]);

#endif //TECHSHELL_COMMAND_H
