// lexer.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "lexer_output.h"
#include "token.h"
#include "utilities.h"

extern token alph(token temp);
extern token alphatoken();
FILE *f = NULL;
const char* filen;
bool done = false;

// Line and column variable.
unsigned int line = 1;
unsigned int col = 1;

// Open the file.
extern void lexer_open(const char *fname)
{
    f = fopen(fname, "r");
    
    // If unreadable file or null, print error (stderr).
    if (f == NULL)
    {
    	fflush(stdout);
    	fprintf(stderr, "Cannot open file: '%s''", fname);
        exit(EXIT_FAILURE);
    }
}


// Close the file.
extern void lexer_close()
{
    int check = fclose(f);
    f = NULL;

    // File did not close and needs to exit with failure.
    if (check == EOF)
    {
        fflush(stdout);
        fprintf(stderr, "Cannot close stream for program file");
        exit(EXIT_FAILURE);
    }
    done = true;
}


// Determine if lexer's token stream is finished.
extern bool lexer_done()
{
    return done;
}


// Return the next token in the input file,
// advancing in the input
extern token lexer_next()
{
    token temp;

    //"uninitialized use" error, so im just initializing
    temp.typ = 0;
    temp.line = line;
    temp.column = col;
    temp.text = (char *) malloc(MAX_IDENT_LENGTH);
    temp.value = 0;

    // Read in character.
    char ch = getc(f);
    
    if (ch == EOF)
    {
        lexer_close();
        temp.typ = 33;
        temp.text = NULL;
        return temp;
    }

    // Illegal character.
    if (ch == '\'')
    {
        lexer_close();
        lexical_error(filen, line, col, "Illegal character '%c' (047)", ch);
    }

    
    // Use isalpha() to determine if the character is a letter.
    if (isalpha(ch) > 0)
    {
        ungetc(ch, f);
        temp = alphatoken(f);
        return temp;
    }

    //determine if the character is a number
    else if (isdigit(ch) > 0)
    {
        ungetc(ch, f);
        int x = 0;
        int startColumn = col;
        while (1)
        {
            x *= 10;
            ch = fgetc(f);

            if (isdigit(ch)>0)
            {
                x += ch - '0';
            }
            
            else
            {
                ungetc(ch, f);
                break;
            }

            col++;
        }

        x /= 10;

        // Outside range of short int.
        if (x < SHRT_MIN || x > SHRT_MAX)
        {
            lexer_close();
            lexical_error(filen, line, startColumn, "The value of %d is too large for a short!", x);
        }

        temp.typ = 22;
        temp.line = line;
        temp.column = startColumn;
        temp.value = x;
    }


    // Other tokens.
    else
    {
        switch(ch)
        {
            // Ignore these characters, so do not print output.
            // Blank, tab, vertical tab, formfeed, newline, carriage return, comment
            // Blank space.
            case ' ':
                // temp.column = col++;
                col++;
                temp.line = line;
                temp = lexer_next();
                break;

            // Tab.
            case '\t':
                col+=4;
                temp = lexer_next();
                break;

            // New line.
            case '\n':
                line++;
                col = 1;
                temp = lexer_next();
                break;
            
            case ',':
                temp.typ = 3;
                temp.text = ",";
                col++;
                break;
            
            // Carriage return.
            case '\r':
                col = 1;
                temp = lexer_next();
                break;
        
            // Comment.
            case '#':

                // Ignore characters until we reach a '\n'.
                while (ch != '\n')
                {
                    ch = getc(f);
                    col++;

                    if (ch == EOF)
                    {
                      lexical_error(filen, line, col, "File ended while reading comment!");
                    }
                }

                line++;
                col = 1;
                temp = lexer_next();
                break;
            
            // Equal symbol.
            case '=':
                temp.typ = 23;
                temp.line = line;
                temp.column = col++;
                temp.text = "=";
                break;

            // Plus symbol.
            case '+':
                temp.typ = 29;
                temp.line = line;
                temp.column = col++;
                temp.text = "+";
                break;

            // Minus symbol.
            case '-':
                temp.typ = 30;
                temp.line = line;
                temp.column = col++;
                temp.text = "-";
                break;

            // Multiplication symbol.
            case '*':
                temp.typ = 31;
                temp.line = line;
                temp.column = col++;
                temp.text = "*";
                break;

            // Divide symbol.
            case '/':
                temp.typ = 32;
                temp.line = line;
                temp.column = col++;
                temp.text = "/";
                break;
            
            //semicolon
            case ';':
                temp.typ = 2;
                temp.line = line;
                temp.column = col++;
                temp.text = ";";
                break;
            
            //becomesym
            case ':':
                temp.line = line;
                temp.column = col++;
                ch = fgetc(f);

                if (ch == '=')
                {
                    temp.typ = 6;
                    temp.text = ":=";
                    col++;
                }

                else
                {
                    lexical_error(filen, line, col,  "Expecting '=' after a colon, not '%c'", ch);
                }
                break;

            //less than, less than or equal, or that <> thing
            case '<':
                ch=fgetc(f);
                if (ch=='=')//less than or equal to (<=)
                {
                    temp.typ = 26;
                    temp.line = line;
                    temp.column = col;
                    temp.text = "<=";
                    col += 2;
                }

                else if(ch=='>')//not equal to (<>)
                {
                    temp.typ = 24;
                    temp.line = line;
                    temp.column = col;
                    temp.text = "<>";
                    col += 2;
                }

                else//it's probably just the less than sign
                {
                    ungetc(ch, f);
                    temp.typ = 25;
                    temp.line = line;
                    temp.column = col++;
                    temp.text = "<";
                }
                break;

            //greater than/greater than or equal
            case '>':
                ch = fgetc(f);

                if(ch=='=')//greater or equal (>=)
                {
                    temp.typ = 28;
                    temp.line = line;
                    temp.column = col;
                    temp.text = ">=";
                    col += 2;
                }

                else
                {
                    ungetc(ch, f);
                    temp.typ = 27;
                    temp.line = line;
                    temp.column = col++;
                    temp.text = ">";
                }
                break;

            case '(':
                temp.typ = 19;
                temp.line = line;
                temp.column = col++;
                temp.text = "(";
                break;

            case ')':
                temp.typ = 20;
                temp.line = line;
                temp.column = col++;
                temp.text = ")";
                break;
            
            case '.':
                temp.typ = 0;
                temp.line = line;
                temp.column = col++;
                temp.text = ".";
                break;
        }
    }
    
    return temp;
}


// Function for identifier, 
extern token alphatoken(FILE *f)
{
    token temp;
    temp.line = line;
    temp.column = col;
    temp.text = (char *) malloc(MAX_IDENT_LENGTH);
    char ch = getc(f);
    char ident[MAX_IDENT_LENGTH] = "";
    int i = 0;

    // Append character to identifier until we read in 
    // a character that is not a letter or digit.
    while (isalpha(ch) > 0 || isdigit(ch) > 0)
    {
        if (ch == '\'')
        {
            lexer_close();
            lexical_error(filen, line, col, "Illegal character '%c' (047)", ch);
        }
        if (i >= MAX_IDENT_LENGTH)
        {
            lexical_error(filen, line, col, "Identifier starting \"%s\" is too long!", ident);
        }

        strncat(ident, &ch, 1);
        ch = getc(f);
        i++;
    }

    if (i > MAX_IDENT_LENGTH)
    {
        lexical_error(filen, line, col, "Identifier starting \"%s\" is too long!", ident);
    }
    col+=i;

    int count = strlen(ident);
    strncpy(temp.text, ident, count);
    temp.typ = 0;
    temp = alph(temp);
    
    if(temp.typ!=0)
    {
        ungetc(ch, f);
        return temp;
    }

    else if(strcmp(ident, "procedure")==0)//alph call wont work on "procedure" so make separate case
    {
        ungetc(ch, f);
        temp.typ = 5;
        return temp;
    }

    else//it's ident otherwise
    {
        ungetc(ch, f);
        temp.typ = 21;
        return temp;
    }

    return temp;
}

//sets temp.typ based on the token
extern token alph(token temp)
{
    for(int i=0; i<34; i++)
    {
        if(strcmp(ttyp2str(i), temp.text)==115)
        {
            temp.typ=i;
            return temp;
        }
    }
    return temp;
}


extern const char *lexer_filename()
{
    return filen;
}


extern unsigned int lexer_line()
{
    return line+1;
}


extern unsigned int lexer_column()
{
    // call this function whenever we are returning a token (with lexer_next)
    // incrementing the column variable based on the token so we're starting
    // at the right place for the next token
    return col+1;
}


// Helper function. 
void lexer(const char *fname)
{
    // Run lexer.
    lexer_open(fname);
    filen = fname;
    lexer_output();
}