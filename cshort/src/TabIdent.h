#ifndef _TAB_IDENT_
#define _TAB_IDENT_

#include <stdbool.h>
#include "Analex.h"

#define MAX_IDENTIF 100

typedef enum {
    CAT_VAR,
    CAT_FUNC,
    CAT_PARAM
} CATEGORIA_IDENTIFICADOR;

typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_CHAR,
    TIPO_BOOL
} TIPO_DADO;

typedef struct {
    char nome[TAMANHO_MAX_LEXEMA];
    CATEGORIA_IDENTIFICADOR categoria; 
    TIPO_DADO tipo; // Tipo de dado do símbolo
    int escopo; // Escopo do símbolo (global, local)
    bool zombie; // Se está fora do escopo
} IDENTIFICADOR;

/* Variáveis globais*/
extern IDENTIFICADOR tabela_id[MAX_IDENTIF];
extern int topo_tabela;

/* Assinatura das Funções */
void inicializarTabela();
void inserirId(const char *nome, CATEGORIA_IDENTIFICADOR categoria, TIPO_DADO tipo, int escopo, bool zombie);
void buscarId(const char *nome);

#endif 