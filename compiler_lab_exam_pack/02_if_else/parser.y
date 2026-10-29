%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void yyerror(const char *);
int yylex(void);
static int tempno=1, labelno=1;
static char *newtemp(void) { char b[32]; snprintf(b,sizeof b,"t%d",tempno++); return strdup(b); }
static char *newlabel(void) { char b[32]; snprintf(b,sizeof b,"L%d",labelno++); return strdup(b); }
%}
%union { char *text; }
%token IF ELSE ASSIGN SEMI LPAREN RPAREN LBRACE RBRACE
%token <text> ID NUM RELOP
%type <text> expr
%left '+' '-'
%left '*' '/'
%%
program : if_stmt { fprintf(stderr, "Syntax correct.\n"); }
        ;
if_stmt : IF LPAREN expr RELOP expr RPAREN
          {
            char *end = newlabel();
            /* Keep the label for the else branch in a global stack-like variable. */
            printf("ifFalse %s %s %s goto %s\n",$3,$4,$5,end);
            $<text>$ = end;
          }
          LBRACE stmt_list RBRACE
          {
            char *done = newlabel();
            printf("goto %s\n%s:\n",done,$<text>7);
            $<text>$ = done;
          }
          ELSE LBRACE stmt_list RBRACE
          { printf("%s:\n", $<text>11); }
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
    freopen("tac.txt","w",stdout);
    if (yyparse() == 0) return 0;
    return 1;
}
