#ifndef ANASINT_H
#define ANASINT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "Analex.h"
#include "TabIdent.h"
#include "Anasem.h"
#include "GeradorCodigo.h"

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
int corpo_func();
void Decl_var();
void Tipo();
void Tipos_param();
void Cmd();
void cmd_cont(TOKEN id_alvo);
void Atrib();
void fator_cont(IDENTIFICADOR func_id);

// Funções que agora retornam o tipo da expressão analisada
TIPO_DADO Expr(); 
TIPO_DADO Expr_simp();
TIPO_DADO Termo();
TIPO_DADO Fator();

void Op_rel();

/* Funções utilitárias */

void trata_array();
bool isOp_rel(TOKEN token);

extern TABELA_IDENTIFICADOR tabelaIdentificadores;

#endif // ANASINT_H