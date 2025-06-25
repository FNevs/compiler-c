/*
  Este arquivo irá conter a implementação do Analisador Sintático
  Descendente Recursivo (ASDR). Cada função corresponde a um
  não-terminal da gramática abaixo.

======================================================================
  Regras de Produção da Gramática Cshort (da Especificação v1.0)
======================================================================

<prog>      ::= { <decl> ';' | <func> }

<decl>      ::= <tipo> <decl_var> { ',' <decl_var> }
              | <tipo> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }
              | <void> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }

<decl_var>  ::= <id> [ '[' <intcon> ']' ]

<tipo>      ::= 'char' | 'int' | 'float' | 'bool'

<tipos_param> ::= <void>
                | <tipo> [ '&' ] <id> [ '[' ']' ] { ',' <tipo> [ '&' ] <id> [ '[' ']' ] }

<func>      ::= <tipo> <id> '(' <tipos_param> ')' '{' { <tipo> <decl_var> {',' <decl_var>} ';'} { <cmd> } '}'
              | <void> <id> '(' <tipos_param> ')' '{' { <tipo> <decl_var> {',' <decl_var>} ';'} { <cmd> } '}'

<cmd>       ::= 'if' '(' <expr> ')' <cmd> [ 'else' <cmd> ]
              | 'while' '(' <expr> ')' <cmd>
              | 'for' '(' [ <atrib> ] ';' [ <expr> ] ';' [ <atrib> ] ')' <cmd>
              | 'return' [ <expr> ] ';'
              | <atrib> ';'
              | <id> '(' [ <expr> { ',' <expr> } ] ')' ';'
              | '{' { <cmd> } '}'

<atrib>     ::= <id> [ '[' <expr> ']' ] '=' <expr>

<expr>      ::= <expr_simp> [ <op_rel> <expr_simp> ]

<expr_simp> ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

<termo>     ::= <fator> { ( '*' | '/' | '&&' ) <fator> }

<fator>     ::= <id> [ '[' <expr> ']' ]
              | <intcon> | <realcon> | <charcon>
              | <id> '(' [ <expr> { ',' <expr> } ] ')'
              | '(' <expr> ')'
              | '!' <fator>

<op_rel>    ::= '==' | '!=' | '<=' | '<' | '>=' | '>'

*/

#include <stdio.h>
#include <stdlib.h>
#include "Analex.h"
#include "Anasint.h" 
#include "TabIdent.c"

void Prog() {
    // TODO
}

void Decl() {
    // TODO
}

void Decl_var() {
    // TODO
}

void Tipo() {
    // TODO
}

void Tipos_param() {
    // TODO
}

void Func() {
    // TODO
}

void Cmd() {
    // TODO
}

void Atrib() {
    // TODO
}

void Expr() {
    // TODO
}

void Expr_simp() {
    // TODO
}

void Termo() {
    // TODO
}

void Fator() {
    // TODO
}

void Op_rel() {
    // TODO
}