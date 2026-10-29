%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void yyerror(const char *);
int yylex(void);
static int tempno=1;
static char *newtemp(void) { char b[32]; snprintf(b,sizeof b,"t%d",tempno++); return strdup(b); }
%}
%union { char *text; }
%token ASSIGN SEMI LPAREN RPAREN
%token <text> ID NUM
%type <text> expr
%left '+' '-'
%left '*' '/'
%%
program : stmt_list { fprintf(stderr, "Syntax correct.\n"); }
        ;
stmt_list : statement
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
