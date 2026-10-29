%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void yyerror(const char *);
int yylex(void);
static int tempno = 1, labelno = 1;
static char *newtemp(void) { char b[32]; snprintf(b,sizeof b,"t%d",tempno++); return strdup(b); }
static char *newlabel(void) { char b[32]; snprintf(b,sizeof b,"L%d",labelno++); return strdup(b); }
static void emit(const char *s) { puts(s); }
static char *loop_start, *loop_end;
%}
%union { char *text; }
%token WHILE IF ELSE ASSIGN SEMI LPAREN RPAREN LBRACE RBRACE
%token <text> ID NUM RELOP
%type <text> expr
%left '+' '-'
%left '*' '/'
%%
program : while_stmt { fprintf(stderr, "Syntax correct.\n"); }
        ;
while_stmt : WHILE LPAREN
             { loop_start = newlabel(); loop_end = newlabel(); printf("%s:\n",loop_start); }
             expr RELOP expr RPAREN
             { printf("ifFalse %s %s %s goto %s\n",$4,$5,$6,loop_end); }
             LBRACE stmt_list RBRACE
             { printf("goto %s\n%s:\n",loop_start,loop_end); }
           ;
stmt_list : /* empty */
          | stmt_list statement
          ;
statement : ID ASSIGN expr SEMI { printf("%s = %s\n",$1,$3); }
          ;
expr : expr '+' expr { $$=newtemp(); printf("%s = %s + %s\n",$$,$1,$3); }
     | expr '-' expr { $$=newtemp(); printf("%s = %s - %s\n",$$,$1,$3); }
     | expr '*' expr { $$=newtemp(); printf("%s = %s * %s\n",$$,$1,$3); }
     | expr '/' expr { $$=newtemp(); printf("%s = %s / %s\n",$$,$1,$3); }
     | LPAREN expr RPAREN { $$=$2; }
     | ID { $$=$1; }
     | NUM { $$=$1; }
     ;
%%
void yyerror(const char *s) { fprintf(stderr,"Syntax error: %s\n",s); }
int main(void) {
    FILE *old = stdout;
    FILE *tac = freopen("tac.txt","w",stdout);
    (void)old; (void)tac;
    if (yyparse() == 0) return 0;
    return 1;
}
