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
#include "shell.h"
#include "command.h"

#define ARBITRATY_TOKEN_BUF_SIZE 16

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int NUM_TOKENS = 0;

/**
 * The main function of the shell.
 */
void enter_shell() {
	//Set the working directory of the shell to the user's home folder based on OS
	if (GET_WINDOWS_HOMEPATH) {
		chdir(getenv("HOMEPATH"));
	} else if (GET_UNIX_HOMEPATH) {
		chdir("~");
	}

	char buf[1024];
	char pathbuf[256];

	while (1) {
		//The flag that denotes whether the string being parsed contains a redirection operator
		bool HAS_REDIRECT = false;
		//The array that stores the redirects in the strings
		redirec_t redirects[] = {NONE, NONE, NONE};
		//An array of strings that contains each token after the command string is processed.
		char** tokenized_input = (char**) malloc(sizeof(char*) * ARBITRATY_TOKEN_BUF_SIZE);
		for (int i = 0; i < 16; i++) {
			tokenized_input[i] = (char*) malloc(sizeof(char) * ARBITRATY_TOKEN_BUF_SIZE * 4);
		}
		//An array of strings that contains the corresponding file for each redirect operator.
		char** filenames = (char**) malloc(sizeof(char*) * E_REDIRECT_N);
		for (int i = 0; i < E_REDIRECT_N; i++) {
			filenames[i] = (char*) malloc(sizeof(char) * 64);
		}
		//The number of redirects the string contains.
		int num_redirects;

		db_printf("enter_shell 1");
		//Writes the current working directory to pathbuf.
		getcwd(pathbuf, sizeof(pathbuf));
		//Print the current working directory to pretty it up for the user
		printf("%s$ ", pathbuf);
		//Get whatever user input and put it in the buf
		gets(buf);
		db_printf("enter_shell 2");
		//Parse the input strings into tokens, redirects, and filenames
		if (parse_cl(buf, tokenized_input, &HAS_REDIRECT, redirects, &num_redirects, filenames) == 0) {
			db_printf("enter_shell 3");
			if (DEBUG) {
				for (int i = 0; i < E_REDIRECT_N; i++) {
					printf("redirects[%d]=%s, filename=%s\n", i, redirect_to_str(redirects[i]), filenames[i]);
				}
			}
			//Execute whatever command the user wants
			execcmd(tokenized_input, HAS_REDIRECT, redirects, num_redirects, filenames);
		} else {
			printf("Parse error\n");
		}

		//malloc'd those arrays so I have to free them
		free(filenames);
		free(tokenized_input);
		//Clear the input buffer
		memset(buf, 0, sizeof buf);
	}
}

#pragma clang diagnostic pop

//BEGIN DEVIN KNUD-SECTION
/**
 * The function that parses the user's input.
 *
 * @param input the input string.
 * @param ret the "return" array that will contain the tokens.
 * @param has_redirect the "return" bool that indicates whether or not the string contains a redirect operator.
 * @param redirects the "return" array that will contain the redirects, if there are any.
 * @param ret_n_redirects the "return" integer that will contain the number of redirects.
 * @param filenames the "return" array that will contain the filenames to each redirect operator.
 * @return 0 on success. -1 if an error is encountered.
 */
int parse_cl(char input[], char** ret, bool* has_redirect, redirec_t redirects[E_REDIRECT_N], int* ret_n_redirects,
			 char** filenames) {
	unsigned int tok_index = 0;
	char* token;
	char* remainder = input;
	//The number of redirects and file names.
	unsigned int num_redirects = 0, num_filenames = 0;
	//The bool that indicates whether or not we are processing redirects, which always appear at the end of command strings.
	bool processing_redirects = false;

	//The main tokenizer loop.
	while ((token = strtok_r(remainder, " ", &remainder))) {
		if (is_redirect(token)) {
			//If a redirect is encountered, update the redirect flags.
			(*has_redirect) = true;
			processing_redirects = true;
			//Get the redirec_t value of the token.
			redirec_t redirect = str_to_redirect(token);

			if (num_redirects == 3) {
				printf("Too many redirects. (Up to 3 are supported)\n");
				return -1;
			}

			redirects[num_redirects++] = redirect;
			continue;
		} else if (!(is_redirect(token)) && processing_redirects) {
			filenames[num_filenames++] = token;
			continue;
		}

		ret[tok_index++] = token;
	}

	//If there are redirects without filenames, error out.
	if (num_filenames != num_redirects) {
		if (DEBUG) {
			printf("num_redirects=%d, num_filenames=%d\n", num_redirects, num_filenames);
		}
		printf("Redirect syntax error (missing filename?)\n");
		return -1;
	}

	NUM_TOKENS = tok_index;
	(*ret_n_redirects) = num_redirects;
	return 0;
}

void execcmd(char** command, const bool has_redirect, const redirec_t redirects[], int n_redirects, char* filenames[]) {
	//Relatively complex macro.
	//Very legal and very cool.
	//All you need to know is that this expands to a macro
	//that individually checks all 3 structs in command.h
	//to see if command[0] matches any of their cmd_str's.
	//If it does, it runs that command and then returns early.
	chk_builtins();

	db_printf("Execcmd 2");

	//The variables that keep track of the opened file descriptors to close them later.
	//This is updated by the getfdesc macro (most of the time).
	int file_descs[3] = {0}; //Init each element to 0
	int desc_num = 0;
	int file_desc;

	if (fork() == 0) {
		if (has_redirect) {
			for (int i = 0; i < n_redirects; i++) {
				db_printf("Setting up redirect file")

				switch (redirects[i]) {
					case STDOUT: {
						getfdesc(OVERWRITE);
						PIPEF_TO(STDOUT_FILENO);
						break;
					}
					case STDERR: {
						getfdesc(OVERWRITE);
						PIPEF_TO(STDERR_FILENO);
						break;
					}
					case STDIN: {
						//I don't use the getfdesc macro here because I don't need to specify any permissions.
						file_desc = open(filenames[i], O_RDONLY);
						file_descs[desc_num++] = file_desc;
						PIPEF_TO(STDIN_FILENO);
						break;
					}
					case STDOUT_APPEND: {
						getfdesc(APPEND);
						PIPEF_TO(STDOUT_FILENO);
						break;
					}
					case STDERR_APPEND: {
						getfdesc(APPEND);
						PIPEF_TO(STDERR_FILENO);
						break;
					}
					case STDOUT_STDERR: {
						getfdesc(OVERWRITE);
						PIPEF_TO(STDOUT_FILENO);
						PIPEF_TO(STDERR_FILENO);
						break;
					}
					case NONE:
						break;
				}
			}
		}

		db_printf("Execcmd 2a");
		char* cmd[NUM_TOKENS];
		cmd[NUM_TOKENS] = NULL;
		for (int i = 0; i < NUM_TOKENS; i++) {
			cmd[i] = command[i];
		}

		db_printf("Execcmd 2b");

		if (execvp(command[0], cmd) == -1) {
			perror("Execution Error");
			exit(EXIT_FAILURE);
		}

		//Close the open file descriptors
		for (int i = 0; i < n_redirects; i++) {
			if (file_descs[i] != 0) {
				close(file_descs[i]);
			}
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

/**
 * Return the redirec_t for the string.
 * @param redirect the redirect string
 * @return the REDIRECT enum type the string represents
 */
redirec_t str_to_redirect(char* redirect) {
	if (cmp_redir_2(">>")) {
		return STDOUT_APPEND;
	} else if (cmp_redir_2("2>>")) {
		return STDERR_APPEND;
	} else if (cmp_redir_2("<")) {
		return STDIN;
	} else if (cmp_redir_2("2>")) {
		return STDERR;
	} else if (cmp_redir_2("&>")) {
		return STDOUT_STDERR;
	} else if (cmp_redir_2(">")) {
		return STDOUT;
	}

	printf("Error: %s is not a redirect", redirect);
	exit(0);
}

/**
 * Convert an enum
 * @param redirect
 * @return
 */
char* redirect_to_str(redirec_t redirect) {
	switch (redirect) {
		case STDOUT_APPEND:
			return ">>";
		case STDOUT:
			return ">";
		case STDERR_APPEND:
			return "2>>";
		case STDIN:
			return "<";
		case STDERR:
			return "2>";
		case STDOUT_STDERR:
			return "&>";
		case NONE:
			return "(NONE)";
		default:
			return NULL;
	}
}