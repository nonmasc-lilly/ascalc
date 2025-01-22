#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lex.h"
#include "machine.h"

void help_menu(void) {
        puts("-- ascalc --");
        puts("Ascalc is a Reverse Polish Notation calculator. Each operation acts on the stack, we may push values and pop values from the stack.");
        puts("A call of ascalc has its arguments specified by a preprocessor on the arguments which are provided by the terminal. This preprocessor");
        puts("has the ability to include files via the -file directive. It is used like so:");
        puts("\n\tasc -file=<file name>\n");
        puts("This shall substitute the argument (-file) with the program which is specified within the file. The remainder of this menu specifies");
        puts("the form of these programs.");
        puts("A number may be specified in one of two forms: decimal or hexadecimal. A decimal number is specified simply by a series of decimal");
        puts("digits (0-9) without breakage, a hexadecimal digit is specified by a series of hexadecimal digits (0-9, A-F) prefixed either by `0x`");
        puts("(as in C) or `$` (as in Pascal). When a number is given to the machine it is simply pushed onto the stack.");
        puts("We can change the value of numbers with a variety of operations. Of these operations there are three kinds:");
        puts("\n\t1. Unary operators, which pop the first value from the stack before doing some operation and returning it. This popped value");
        puts("\t   shall be named `a`.");
        puts("\n\t2. Binary operators, which pop the first two values from the stack before doing some operation between them and returning its value.");
        puts("\t   The second value to pop shall be named a, and the first b.");
        puts("\n\t3. Ternary operators, which pop the first three values from the stack before doing some operation between them and returning its value.");
        puts("\t   The third value to pop shall be named a, the second b, and the third c.\n");
        puts("This unorthodox ordering is to allow the statement `5 3 +` to mean `5 + 3` rather than `3 + 5`. To finish off this listing we may state");
        puts("that the unary operator M is to be used as: `a M`, the binary operator N is to be used as: `a b N` and the ternary operator O is to be");
        puts("used as: `a b c O`. In the definitions which follow we shall use the following lexical convention for defining operations:");
        puts("\n\t<purpose of a> <purpose of b> <purpose of c> name/symbol\n");
        puts("Without further adieu, we shall commence the definition of the Unary operations:");
        puts("\n\t<argument> ~\n");
        puts("Bitwise not simply inverts all of the bits of a.");
        puts("\n\t<argument> !\n");
        puts("Logical not simply assigns a to zero if it is nonzero and to one if it is zero.");
        puts("\n\t<to print> hex\n");
        puts("Hex simply prints the number on top of the stack as a hexadecimal value (prefixed with $).");
        puts("\n\t<to print> print\n");
        puts("Print simply prints the number on top of the stack as a decimal value.");
        puts("\n\t<number of sectors per track> sectors\n");
        puts("Sectors sets the number of sectors per track for use with the `@` command (ternary operator) defaults to 63.");
        puts("\n\t<number of heads per cylinder> heads\n");
        puts("Heads sets the number of heads per cylinder for use with the `@` command (ternary operator) defaults to 255.");
        puts("\nBinary operators:");
        puts("\n\t<a> <b> add/+\n");
        puts("Add simply returns a+b");
        puts("\n\t<a> <b> sub/-\n");
        puts("Sub simply returns a-b");
        puts("\n\t<a> <b> mul/*\n");
        puts("Mul simply returns a*b");
        puts("\n\t<a> <b> div or /\n");
        puts("Div simply returns a/b");
        puts("\n\t<a> <b> mod/%%\n");
        puts("Mod simply returns the remainder of a/b");
        puts("\n\t<a> <b> and/&\n");
        puts("And performs a bitwise and between a and b");
        puts("\n\t<a> <b> or/|\n");
        puts("Or performs a bitwise or between a and b");
        puts("\n\t<a> <b> shl/<\n");
        puts("Shl multiplies a by 2^b by shifting each bit of a up one");
        puts("\n\t<a> <b> shr/>\n");
        puts("Shr divides a by 2^b by shifting each bit of a up one (note that this is an unsigned division and shant work properly for signed number)");
        puts("\n\t<segment> <offset> seg/:\n");
        puts("Seg simply calculates the 8086 segment address corresponding to `a:b`");
        puts("\nTernary operators:");
        puts("\n\t<a> <b> <condition> cond/?\n");
        puts("The conditional operator simply returns a if c is nonzero and b otherwise.");
        puts("\n\t<c> <h> <s> chs/@\n");
        puts("The CHS operator calculates the LBA equivalent to a given CHS address.");
        exit(0);
}

int main(int argc, char **argv) {
        uint32_t        i;
        STATE       state;
        TOKENS     tokens;
        char     *content;
        if(argc < 2) {
                fprintf(stderr, "Expected arguments (use -help) for help.\n");
                exit(1);
        }
        content = calloc(1, 1);
        for(i = 1; i < argc; i++) {
                FILE *fp;
                uint32_t length;
                if(memcmp(argv[i], "-file=", 6)) {
                        if(!strcmp(argv[i], "-help")) {
                                free(content);
                                help_menu();
                        }
                        content = realloc(content, strlen(content) + strlen(argv[i]) + 2);
                        strcat(content, " ");
                        strcat(content, argv[i]);
                        continue;
                }
                
                fp = fopen(argv[i]+6, "r");
                if(!fp) {
                        fprintf(stderr, "Could not open file `%s` for reading, exitting...\n");
                        free(content);
                        exit(1);
                }
                fseek(fp, 0L, SEEK_END);
                length = ftell(fp);
                content = realloc(content, strlen(content)+length+1);
                content[strlen(content)+length] = 0;
                fseek(fp, 0L, SEEK_SET);
                fread(content+strlen(content), 1, length, fp);
                fclose(fp);
        }
        tokens_create(&tokens);
        state_create(&state);

        lex(&tokens, content);
        simulate_machine(&tokens, &state);

        state_destroy(&state);
        tokens_destroy(&tokens);
        exit(0);
}
