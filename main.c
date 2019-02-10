#include <stdio.h>
#include "command.h"
#include "shell.h"

int main(int argc, const char* argv[]) {
    init_cmd_structs();
    enter_shell();
    return 0;
}