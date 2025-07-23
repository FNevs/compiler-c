#include "GeradorCodigo.h"
#include <stdarg.h> 
#include <stdlib.h> 
#include <stdio.h>

static FILE* arquivo_saida;

static int contador_rotulo = 0;

/**
 * Implementação da função que abre o arquivo de saída.
 */
void inicializar_gerador(const char* nome_arquivo) {
    arquivo_saida = fopen(nome_arquivo, "w"); // Abre o arquivo em modo de escrita ("w")
    if (arquivo_saida == NULL) {
        // Se o arquivo não puder ser criado, exibe um erro e encerra o compilador.
        printf("Erro: Nao foi possivel criar o arquivo de saida '%s'.\n", nome_arquivo);
        exit(1);
    }
}

/**
 * Implementação da função que fecha o arquivo de saída.
 */
void finalizar_gerador() {
    if (arquivo_saida != NULL) {
        fclose(arquivo_saida);
        arquivo_saida = NULL; 
    }
}

/**
 * Implementação da função principal de geração de código.
 */
void gerar_codigo(const char* formato, ...) {
    if (arquivo_saida == NULL) {
        return;
    }

    va_list args;
    va_start(args, formato);

    vfprintf(arquivo_saida, formato, args);

    fprintf(arquivo_saida, "\n");

    va_end(args);
}

/**
 * Implementação da função que cria um novo rótulo.
 */
void novo_rotulo(char* rotulo) {

    sprintf(rotulo, "L%d", contador_rotulo++);
}