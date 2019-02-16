#Taco Shell

Contributors: Devon Knudsen / Brennan Forrest
Class: CSC-222-002

#Features
Taco Shell includes **only** the following features:

* Basic command execution (no piping)
* Output and input redirection

All redirects supported in Bourne-Again Shell 5 are supported in Taco Shell.

#Logistics

Most everything occurs in the shell.c file. This file
contains the function that tokenizes the input string into
a command, its arguments, and its redirects and the files they
redirect to. It also contains the function that forks the process
and executes the command. Many macros were created to simplify the
code, and they can be found in shell.h.

Due to its low number of features, each feature received considerable
development and testing time, leading to a relatively low-bug experience.
The parse function properly allocates and frees the arrays that store 
file names and tokens for each command, and closes any opened file
descriptors after completing execution.

The following commands cannot be redirected:
cd
pwd
exit

because they are handled separately from all other commands. 