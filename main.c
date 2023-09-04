// Main file

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

// Function declarations.
void lexer(const char *fname);

int main (int argc, char **argv)
{
    // Call helper function to run the program.
    lexer(argv[1]);

    return EXIT_SUCCESS;
}