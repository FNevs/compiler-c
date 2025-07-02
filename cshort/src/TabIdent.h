#ifndef _TAB_IDENT_
#define _TAB_IDENT_

#include <stdbool.h>
#include "Analex.h"

#define MAX_IDENTIF 100

typedef enum {
    CAT_VAZIA,
    CAT_VAR,
    CAT_FUNC,
    CAT_PARAM
} CATEGORIA_IDENTIFICADOR;

typedef enum {
    TIPO_VAZIO,
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
    int endereco;
    int tamArray;
    bool array;
    bool zombie; // Se está fora do escopo
} IDENTIFICADOR;

typedef struct {
    int tamTabela;
    IDENTIFICADOR identificadores[MAX_IDENTIF];
} TABELA_IDENTIFICADOR;

/* Variáveis globais*/
extern TOKEN t;
extern TOKEN lookahead;
extern TABELA_IDENTIFICADOR tabelaIdentificadores;

/* Assinatura das Funções */
void IniciaTabelaID();
int BuscaTabelaID(const char *nomeId);
int BuscaTabelaIDMesmoEscopo(const char *nomeId, int escopo);
int InsereTabelaID(const char *nomeId, CATEGORIA_IDENTIFICADOR catId, int escopo, TIPO_DADO tipo, bool isZombie, bool isArray, int tamArray);
int removeUltimoIDInserido(int posicaoUltimoInserido);

#endif 