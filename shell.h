//
// Created by bzane on 2/10/2019.
//
#include <stdbool.h>

#ifndef TECHSHELL_SHELL_H
#define TECHSHELL_SHELL_H

//Initial DEFINE values for OS detection
#define GET_WINDOWS_HOMEPATH 0
#define GET_LINUX_HOMEPATH 0

//For any version of windows, including Cygwin dev environments
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#undef GET_WINDOWS_HOMEPATH
#define GET_WINDOWS_HOMEPATH 1
#endif //_WIN32

//For any version of *nix
#if defined(unix) || defined(__unix__) || defined (__unix)
#undef GET_UNIX_HOMEPATH
#define GET_UNIX_HOMEPATH 1
#endif //unix/__unix__/__unix

#define E_REDIRECT_N 3
typedef enum REDIRECT {
	NONE,
	STDIN,
	STDOUT,
	STDERR,
	STDOUT_APPEND,
	STDERR_APPEND,
	STDOUT_STDERR
} redirec_t;

void enter_shell(void);

int parse_cl(char[], char* [], bool*, redirec_t[], int*, char* []);

void execcmd(char**, bool, const redirec_t[], int, char* []);

redirec_t str_to_redirect(char*);

char* redirect_to_str(redirec_t);

//Useful macros
#define command_compare(s) strcmp(command[0], s->cmd_str) == 0
#define call(cmd) (cmd->function)((const char**) command);
#define chk_builtin(command) if (command_compare(command)) { call(command); return; }
#define chk_builtins() chk_builtin(p_wd); chk_builtin(cd); chk_builtin(exit_cmd);
#define cmp_redir(redirect, s) strcmp(redirect, s) == 0
#define cmp_redir_2(s) cmp_redir(redirect, s)
#define is_redirect(s) cmp_redir(">>", s) || cmp_redir("2>>", s) || cmp_redir("2>", s) || cmp_redir("<", s) || cmp_redir("&>", s) || cmp_redir(">", s)

#define db_printf(s) if (DEBUG) { printf("%s\n", s); }

#define OVERWRITE O_CREAT | O_WRONLY
#define APPEND O_CREAT | O_APPEND
#define PERMS 0666

//This macro is for use only in shell.c/execcmd function.
//It updates the file_desc variable for use in dup2 and also puts it in the file_descs array so they can be closed cleanly.
#define getfdesc(mode) file_desc = open(filenames[i], mode, PERMS); file_descs[desc_num++] = file_desc
//This is also only for use in shell.c/execcmd.
//this just calls dup2 on the file_desc variable and whatever IO pipe is specified.
#define PIPEF_TO(fileno) dup2(file_desc, fileno)
//DEBUG macro for this file
#define DEBUG 0

#endif //TECHSHELL_SHELL_H
