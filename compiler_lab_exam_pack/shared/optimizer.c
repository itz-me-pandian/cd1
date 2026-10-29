#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 2000
#define LINE 512
static char lines[MAX_LINES][LINE];
static int nlines;

static int is_int(const char *s) {
    if (!s || !*s) return 0;
    if (*s == '-') s++;
    if (!*s) return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

static int line_assigns_name(const char *line, const char *name) {
    char lhs[128];
    if (sscanf(line, " %127[^= ] =", lhs) == 1)
        return strcmp(lhs, name) == 0;
    return 0;
}

static void fold_line(char *s) {
    char lhs[128], a[128], b[128], op, extra;
    long x, y, z;
    int got = sscanf(s, " %127[^= ] = %127s %c %127s %c", lhs, a, &op, b, &extra);
    if (got < 4 || !is_int(a) || !is_int(b)) return;
    x = strtol(a, NULL, 10);
    y = strtol(b, NULL, 10);
    switch (op) {
        case '+': z = x + y; break;
        case '-': z = x - y; break;
        case '*': z = x * y; break;
        case '/': if (y == 0) return; z = x / y; break;
        default: return;
    }
    snprintf(s, LINE, "%s = %ld", lhs, z);
}

int main(int argc, char **argv) {
    FILE *in, *out;
    int loop_start = -1, cond_line = -1, loop_end = -1;
    int hoist[MAX_LINES] = {0};

    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.tac output.tac\n", argv[0]);
        return 1;
    }
    in = fopen(argv[1], "r");
    if (!in) { perror(argv[1]); return 1; }
    while (nlines < MAX_LINES && fgets(lines[nlines], LINE, in)) {
        lines[nlines][strcspn(lines[nlines], "\r\n")] = '\0';
        nlines++;
    }
    fclose(in);

    /* Pass 1: constant folding */
    for (int i = 0; i < nlines; i++) fold_line(lines[i]);

    /* Pass 2: conservative loop-invariant code motion for the first loop.
       Find the loop's entry label, conditional exit, and exit label. */
    for (int i = 0; i < nlines; i++) {
        if (lines[i][0] == 'L' && strchr(lines[i], ':')) {
            for (int j = i + 1; j < nlines; j++) {
                if (strncmp(lines[j], "ifFalse ", 8) == 0) {
                    char target[64];
                    if (sscanf(strstr(lines[j], "goto ") ? strstr(lines[j], "goto ") : "",
                               "goto %63s", target) == 1) {
                        loop_start = i;
                        cond_line = j;
                        for (int k = j + 1; k < nlines; k++) {
                            char label[80];
                            snprintf(label, sizeof label, "%s:", target);
                            if (strcmp(lines[k], label) == 0) {
                                loop_end = k;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            if (loop_start >= 0) break;
        }
    }

    if (loop_start >= 0 && cond_line > loop_start && loop_end > cond_line) {
        for (int i = loop_start + 1; i < loop_end; i++) {
            char lhs[128], a[128], b[128], op, extra;
            int got = sscanf(lines[i], " %127[^= ] = %127s %c %127s %c",
                             lhs, a, &op, b, &extra);
            if (got < 4 || lhs[0] != 't' || !is_int(b)) continue;
            if (!(isalpha((unsigned char)a[0]) || a[0] == '_')) continue;

            /* A variable is invariant only if the loop body does not assign it. */
            int modified = 0;
            for (int k = cond_line + 1; k < loop_end; k++) {
                if (line_assigns_name(lines[k], a)) { modified = 1; break; }
            }
            if (!modified) {
                hoist[i] = 1;
            }
        }
    }

    out = fopen(argv[2], "w");
    if (!out) { perror(argv[2]); return 1; }

    /* Emit hoisted computations before the loop-entry label. */
    if (loop_start >= 0) {
        for (int i = loop_start + 1; i < loop_end; i++)
            if (hoist[i]) fprintf(out, "%s\n", lines[i]);
    }
    for (int i = 0; i < nlines; i++) {
        if (hoist[i]) continue;
        if (lines[i][0]) fprintf(out, "%s\n", lines[i]);
    }
    fclose(out);
    puts("Optimization complete.");
    return 0;
}
