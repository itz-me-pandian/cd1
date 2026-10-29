%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}
%option noyywrap
%%
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.text = strdup(yytext); return ID; }
[0-9]+                  { yylval.text = strdup(yytext); return NUM; }
[+\-*/]                 { return yytext[0]; }
"="                     { return ASSIGN; }
";"                     { return SEMI; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
[ \t\r\n]+              ;
.                       { return yytext[0]; }
%%
