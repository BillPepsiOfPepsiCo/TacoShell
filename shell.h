//
// Created by bzane on 2/10/2019.
//

#ifndef TECHSHELL_SHELL_H
#define TECHSHELL_SHELL_H

#define GET_WINDOWS_HOMEPATH 0
#define GET_LINUX_HOMEPATH 0

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#undef GET_WINDOWS_HOMEPATH
#define GET_WINDOWS_HOMEPATH 1
#define PATH_DELIM "\\"
#endif //_WIN32

#if defined(unix) || defined(__unix__) || defined (__unix)
#undef GET_UNIX_HOMEPATH
#define GET_UNIX_HOMEPATH 1
#define PATH_DELIM "/"
#endif //unix/__unix__/__unix

static char* WORKING_DIR = "ERR";

void enter_shell(void);
int process_external_command(char*[]);

#endif //TECHSHELL_SHELL_H
