#include <stdio.h>
#include <stdlib.h>
#include "TabIdent.h"

void IniciaTabelaID() {
    tabelaIdentificadores.tamTabela = 0;  
}

/*
* @brief Retorna o index do Identificador na Tabela de Identificadores
* @param nomeId Nome do identificador
*/
int BuscaTabelaID(const char *nomeId) {

    for (int i = 0; i < tabelaIdentificadores.tamTabela; i++) {
        if (!(strcmp(nomeId, tabelaIdentificadores.identificadores[i].nome))) {
            return i;
        }
    }
    return -1;
}

int BuscaTabelaIDMesmoEscopo(const char *nomeId, ESCOPO escopo) {

    for (int i = 0; i < tabelaIdentificadores.tamTabela; i++) {
        if (!(strcmp(nomeId, tabelaIdentificadores.identificadores[i].nome))) {
            if( tabelaIdentificadores.identificadores[i].escopo == escopo) {
                return i;
            }
        }
    }

    return -1;
}

int InsereTabelaID(const char *nomeId, CATEGORIA_IDENTIFICADOR catId, ESCOPO escopo, TIPO_DADO tipo, bool isZombie, bool isArray, int tamArray) {
    if (tabelaIdentificadores.tamTabela == MAX_IDENTIF) {
        printf("Estouro na tabela de identificadores!");
    }

    int i = tabelaIdentificadores.tamTabela;
    strcpy(tabelaIdentificadores.identificadores[i].nome, nomeId);

    tabelaIdentificadores.identificadores[i].endereco = i;
    tabelaIdentificadores.identificadores[i].categoria = catId;
    tabelaIdentificadores.identificadores[i].escopo = escopo;
    tabelaIdentificadores.identificadores[i].tipo = tipo;
    tabelaIdentificadores.identificadores[i].zombie = isZombie;
    tabelaIdentificadores.identificadores[i].array = isArray;
    tabelaIdentificadores.identificadores[i].tamArray = tamArray;

    tabelaIdentificadores.tamTabela++;
    return i;
}

int removeUltimoIDInserido(int posicaoUltimoInserido)
{
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].nome[0] = '\0';
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].categoria = CAT_VAZIA;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].tipo = TIPO_VAZIO;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].escopo = GLOBAL;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].endereco = 0;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].array = false;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].tamArray = 0;
    tabelaIdentificadores.identificadores[posicaoUltimoInserido].zombie = false;

    return posicaoUltimoInserido - 1;
}
