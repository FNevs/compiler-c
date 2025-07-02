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
    bool ref;
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
void IniciaTabIdentif();
void EditaQtdParams(const char *nomeId, int parametros);
void EditaNomeArg(const char *nomeId, int index);
void ExcluiVL(const char *nomeId);
void ZombieARG(const char *nomeId);
int BuscaTabIdetif(const char *nomeId);
int BuscaTabIdetifBlk(const char *nomeId);
int InsereTabIdentif(const char *nomeId, CATEGORIA_IDENTIFICADOR catId, int escopo, TIPO_DADO tipo, bool isConst, bool isZombie, bool isRef, int parametros);

#endif 