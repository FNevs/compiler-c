#ifndef ANASINT_H
#define ANASINT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "Analex.h"
#include "TabIdent.h"

typedef enum { NO_DECL, DECL_VAR, DECL_PROT, DECL_PROT_UNICO } DECL_SINALIZADOR;

/* Variáveis Globais */
extern TOKEN t;
extern TOKEN tLookahead;
extern FILE *fd;
extern int contLinha;
extern ESCOPO escopo;

/* Funções das Regras de produção da gramática*/
void Prog();
DECL_SINALIZADOR Decl();
void corpo_func();
void Decl_var();
void Tipo();
void Tipos_param();
void Cmd();
void cmd_cont();
void Atrib();
void Expr(); 
void Expr_simp();
void Termo();
void Fator();
void fator_cont();
void Op_rel();

/* Funções utilitárias */

void trata_array();
bool isOp_rel(TOKEN token);

#endif // ANASINT_H