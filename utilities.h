/* $Id: utilities.h,v 1.2 2023/01/31 08:32:17 leavens Exp $ */
#ifndef _UTILITIES_H
#define _UTILITIES_H

// Format a string error message and print it using perror (for an OS error)
// then exit with a failure code, so a call to this does not return.
extern void bail_with_error(const char *fmt, ...);

// Print a lexical error message to stderr
// starting with the filename, a colon, the line number, a comma
// the column number, a colon, and then the message.
// Output goes to stderr and then an exit with a failure code,
// so a call to this function does not return.
extern void lexical_error(const char *filename, unsigned int line,
			  unsigned int column, const char *fmt, ...);
#endif
