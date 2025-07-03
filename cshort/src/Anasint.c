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

typedef enum { NO_DECL, DECL_VAR, DECL_PROT, DECL_PROT_UNICO } DECL_SINALIZADOR;

void Prog() {

    DECL_SINALIZADOR declFlag;

    t = Analex(fd);
    while (t.cat != FIM_ARQ) {
        declFlag = decl();

        if (t.cat==SN) {
            if (t.codigo==PONTO_VIRGULA) {
                t = Analex(fd);
            }
            else if (t.codigo==ABRE_CHAVES) {

                if (declFlag!=DECL_PROT_UNICO) {
                    erro("Definição de função inválida");
                }

                t = Analex(fd);
                corpo_func();

                if (!(t.cat==SN && t.codigo==FECHA_CHAVES)) {
                    erro("Fecha chaves esperado");
                }

                t = Analex(fd);

            }
            else {
                erro("Ponto e virgula esperado");
            }
        }
    }
}

void Decl() {
    DECL_SINALIZADOR declFlag = NO_DECL;
    int tipo;
    int contProt = 0;

    if (t.cat == PR && 
        (t.codigo == INT || t.codigo == FLOAT || t.codigo == BOOL ||
        t.codigo == CHAR || t.codigo == VOID)) {
        tipo = t.codigo;

        t = Analex(fd);

        if (t.cat != ID) {
            erro("Identificador esperado");
        }

        if (tLookahead.cat==SN && tLookahead.codigo==ABRE_COLCH) {
            declFlag = DECL_VAR;

            if (tipo == VOID) {
                erro("Tipo void inválido");
            }

            trata_dim_array();
        }

        if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) {
            declFlag = DECL_VAR;

            if (tipo == VOID) {
                erro("Tipo void inválido");
            }

            t = Analex(fd);

            while (t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd);
                if (t.cat != ID) {
                    erro("Identificador esperado");
                }

                if (tLookahead.cat==SN && tLookahead.codigo==ABRE_COLCH) {
                    trata_dim_array();
                }
                t = Analex(fd); 
            }
        }
        else if (tLookahead.cat == SN && tLookahead.codigo == ABRE_PAREN) {

            if (declFlag==DECL_VAR) {
                erro("Declaração simultânea de variável e função inválida");
            }

            declFlag = DECL_PROT;
            contProt++;
            t = Analex(fd); // consome ´(´
            escopo = LOCAL;
            t = Analex(fd); 
            tipos_param();

            if (t.cat != SN || t.codigo == FECHA_PAREN) {
                erro("Fecha parênteses de fim de parâmetros de função esperado");
            }

            escopo = GLOBAL;

            if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) {
                t = Analex(fd);
                while (t.cat == SN && t.codigo == VIRGULA) {
                    contProt++;
                    t = Analex(fd);

                    if (t.cat != ID) {
                        erro("Identificador esperado");
                    }

                    t = Analex(fd);

                    if (t.cat != SN || t.codigo != ABRE_PAREN) {
                        erro("Declaração de parâmetros de função esperado");
                    }

                    t = Analex(fd); // consome ´(´
                    escopo = LOCAL;
                    t = Analex(fd); 
                    tipos_param();

                    if (t.cat != SN || t.codigo != FECHA_PAREN) {
                        erro("Fecha parênteses de fim de parâmetros de função esperado");
                    }

                    escopo = GLOBAL;
                    t = Analex(fd);
                }
            }

            if (contProt == 1) {
                declFlag = DECL_PROT_UNICO; 
            }
        } else {
            declFlag = DECL_VAR;
        }
    } else {
        erro("Tipo inválido");
    }

    return declFlag;
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