//
// Created by bzane on 2/10/2019.
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "command.h"

/**
 * Initializes all structs contained in command.h.
 */
void init_cmd_structs() {
	//Init for "cd" command
	cd = (cmd_t*) malloc(sizeof(cmd_t));
	cd->cmd_str = "cd";
	cd->function = _builtin_cd;
	cd->usage_message = "cd <dir>";

	//Init for "pwd" command
	p_wd = (cmd_t*) malloc(sizeof(cmd_t));
	p_wd->cmd_str = "pwd";
	p_wd->function = _builtin_pwd;
	p_wd->usage_message = "pwd (no args)";

	//Init for "exit" command
	exit_cmd = (cmd_t*) malloc(sizeof(cmd_t));
	exit_cmd->cmd_str = "exit";
	exit_cmd->function = _builtin_exit;
	exit_cmd->usage_message = "exit (no args)";
}

/**
 * Builtin command for changing the working directory.
 * Calls chdir in unistd.h and updates WORKING_DIR in shell.h
 * @param dir
 * @return
 */
int _builtin_cd(const char* args[]) {
	char* dir = args[1];
	return chdir(dir);
}

/**
 * Builtin command for printing the current working directory.
 * @return
 */
int _builtin_pwd(const char* args[]) {
	char pathbuf[256];
	getcwd(pathbuf, sizeof(pathbuf));
	printf("%s\n", pathbuf);
	return 0;
}

/**
 * Builtin command for exiting.
 * @return
 */
int _builtin_exit(const char* args[]) {
	exit(0);
}

