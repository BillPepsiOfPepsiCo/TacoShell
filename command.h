//
// Created by bzane on 2/10/2019.
//

#ifndef TECHSHELL_COMMAND_H
#define TECHSHELL_COMMAND_H

/**
 * The structure that stores the name of a command
 * and its associated function.
 */

typedef int (* associated_function)(const char* []);

typedef char* command_string;

/**
 * The struct type for builtin commands.
 */
typedef struct Command {
	command_string cmd_str;
	associated_function function;
	char* usage_message;
} cmd_t;

cmd_t* cd;
cmd_t* p_wd;
cmd_t* exit_cmd;

void init_cmd_structs(void);

int parse(const char* []);

//Built-in commands
int _builtin_cd(const char* []);

int _builtin_pwd(const char* []);

int _builtin_exit(const char* []);

#endif //TECHSHELL_COMMAND_H
