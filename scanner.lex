%{
    #include <stdio.h>
    #include "source.h"
    #define YYSTYPE NonTerminal
    #include "parser.tab.hpp"
    #include "hw3_output.hpp"
%}

%option yylineno
%option noyywrap


relop_equality          ((\=\=)|(\!\=))
relop_relational        ((\<)|(\>)|(\<\=)|(\>\=))
in                      (in)
binop_additive          (\+|\-)
binop_multiplicative    (\*|\/|\%)
comment                 (\/\/[^\r\n]*[\r|\n|\r\n]?)
dec_digit               ([0-9])
hex_digit               ([0-9a-fA-F])
num                     (0|[1-9][0-9]*)
escape_param            ([\\\"nrt0]|(x[0-7]{hex_digit}))
good_string             (\"([^\n\r\"\\]|\\[rnt\"\\])+\")
letter  	            ([a-zA-Z])
id                      ([a-zA-Z][a-zA-Z0-9]*)
new_line                ($|[\r\n])
space                   [\ \t\r\n]

%%


void                        return VOID;
int                         return INT;
byte                        return BYTE;
b                           return B;
bool                        return BOOL;
set                         return SET_def;
and                         return AND;
or                          return OR;
not                         return NOT;
true                        return TRUE;
false                       return FALSE;
return                      return RETURN;
if                          return IF;
else                        return ELSE;
while                       return WHILE;
break                       return BREAK;
continue                    return CONTINUE;
;                           return SC;
,                           return COMMA;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LBRACE;
\}                          return RBRACE;
\[                          return LBRACKET;
\]                          return RBRACKET;
\=                          return ASSIGN;
\.\.                        return DOTS;
{in}                        return IN_RELOP;
{relop_relational}          {
                                yylval = NonTerminal(strdup(yytext), "");
                                //yylval.name = strdup(yytext);
                                return RELOP_RELATIONAL;
                            }
{relop_equality}            {
                                yylval = NonTerminal(strdup(yytext), "");
                                yylval.name = strdup(yytext);
                                return RELOP_EQUALITY;
                            }
{binop_multiplicative}      {
                                yylval = NonTerminal(strdup(yytext), "");
                                yylval.name = strdup(yytext);
                                return BINOP_MULTIPLICATIVE;
                            }
{binop_additive}            {
                                yylval = NonTerminal(strdup(yytext), "");
                                yylval.name = strdup(yytext);
                                return BINOP_ADDITIVE;
                            }
{comment}                   ;
{id}                        {
                                yylval = NonTerminal(strdup(yytext), "");
                                yylval.name = strdup(yytext);
                                return ID;
                            }
{num}                       {
                                yylval = NonTerminal(strdup(yytext), "");
                                yylval.name = strdup(yytext);
                                return NUM;
                            }
{good_string}               {
                            yylval = NonTerminal(strdup(yytext), "");
                            yylval.name = strdup(yytext);
                            return STRING;
                            }
{space}                     ;
.                           {output::errorLex(yylineno); myExit();}

%%


