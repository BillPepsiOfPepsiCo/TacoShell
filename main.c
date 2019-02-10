#include <stdio.h>
#include "command.h"

int main(int argc, const char* argv[]) {
    init_cmd_structs();

    int i = (exit_cmd->function)();
    return 0;
}