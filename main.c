
#include "command.h"
#include "shell.h"

int main(int argc, const char* argv[]) {
	//Init the builtin command structs
	init_cmd_structs();
	//Start the shell
	enter_shell();
	return 0;
}