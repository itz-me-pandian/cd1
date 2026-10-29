#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Emits illustrative accumulator-style pseudo assembly from TAC.
   This is deliberately simple and intended for compiler-lab demonstration. */
int main(int argc, char **argv) {
    FILE *in, *out;
    char line[512], lhs[128], a[128], b[128], op, extra;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.tac output.asm\n", argv[0]);
        return 1;
    }
    in = fopen(argv[1], "r");
    if (!in) { perror(argv[1]); return 1; }
    out = fopen(argv[2], "w");
    if (!out) { perror(argv[2]); fclose(in); return 1; }
    fprintf(out, "; Illustrative target code generated from TAC\n");
    while (fgets(line, sizeof line, in)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (!line[0]) continue;
        if (line[0] == 'L' && strchr(line, ':')) {
            fprintf(out, "%s\n", line);
            continue;
        }
        if (strncmp(line, "goto ", 5) == 0) {
            fprintf(out, "JMP %s\n", line + 5);
            continue;
        }
        if (strncmp(line, "ifFalse ", 8) == 0) {
            char x[128], rel[8], y[128], label[128];
            if (sscanf(line, "ifFalse %127s %7s %127s goto %127s", x, rel, y, label) == 4)
                fprintf(out, "LOAD R0, %s\nCMP R0, %s\nJFALSE_%s %s\n", x, y, rel, label);
            else fprintf(out, "; unrecognized conditional: %s\n", line);
            continue;
        }
        if (sscanf(line, " %127[^= ] = %127s %c %127s %c", lhs, a, &op, b, &extra) == 4) {
            fprintf(out, "LOAD R0, %s\n", a);
            switch (op) {
                case '+': fprintf(out, "ADD R0, %s\n", b); break;
                case '-': fprintf(out, "SUB R0, %s\n", b); break;
                case '*': fprintf(out, "MUL R0, %s\n", b); break;
                case '/': fprintf(out, "DIV R0, %s\n", b); break;
                default: fprintf(out, "; unknown operator %c\n", op); break;
            }
            fprintf(out, "STORE %s, R0\n", lhs);
        } else if (sscanf(line, " %127[^= ] = %127s", lhs, a) == 2) {
            if (isdigit((unsigned char)a[0]) || (a[0] == '-' && isdigit((unsigned char)a[1])))
                fprintf(out, "MOV %s, %s\n", lhs, a);
            else
                fprintf(out, "LOAD R0, %s\nSTORE %s, R0\n", a, lhs);
        } else {
            fprintf(out, "; %s\n", line);
        }
    }
    fclose(in); fclose(out);
    puts("Target code generated.");
    return 0;
}
