#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Analex.h"

// Variáveis globais
static int contLinha = 1; // Contador de linhas

// Função de erro
void error(char *msg) {
    fprintf(stderr, "ERRO LÉXICO (linha %d): %s\n", contLinha, msg);
    exit(1);
}
