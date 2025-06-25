#ifndef ANASINT_H
#define ANASINT_H

#include <stdio.h>
#include <stdbool.h>
#include "Analex.h"
#include "TabIdent.h"

/* Variáveis globais */
extern TOKEN t;
extern TOKEN tLookahead;
extern FILE *fd;
extern int contLinha;

//void parser(); -> uma função de entrada para o analisador sintático
// verificar se é necessário

/* Assinatura das Funções */
void Prog();
void Decl();
void DeclVar();
void Tipo();
void Tipos_param();
void Func();
void Cmd();
void Atrib();
void Expr();
void Expr_simp();
void Termo();
void Fator();
void Op_rel();

#endif // ANASINT_H