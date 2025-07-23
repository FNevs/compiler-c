#include "gerador_codigo.h"
#include <stdarg.h> // Necessário para funções com número variável de argumentos
#include <stdlib.h> // Para a função exit()
#include <stdio.h>

// Ponteiro estático para o arquivo de saída. 'static' significa que esta variável
// só é visível dentro deste arquivo, mantendo o módulo encapsulado.
static FILE* arquivo_saida;

// Contador estático para os rótulos. Garante que cada chamada a novo_rotulo()
// produza um rótulo diferente e único (L0, L1, L2, ...).
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
        arquivo_saida = NULL; // Boa prática: define o ponteiro como nulo após fechar.
    }
}

/**
 * Implementação da função principal de geração de código.
 */
void gerar_codigo(const char* formato, ...) {
    if (arquivo_saida == NULL) {
        // Medida de segurança: não faz nada se o gerador não foi inicializado.
        return;
    }

    // va_list, va_start, va_end são mecanismos do C para lidar com
    // funções que aceitam um número variável de argumentos (como o printf).
    va_list args;
    va_start(args, formato);

    // vfprintf é a versão do fprintf que funciona com uma va_list.
    // Ela escreve a string formatada no arquivo de saída.
    vfprintf(arquivo_saida, formato, args);

    // Adiciona uma quebra de linha após cada instrução para formatar o arquivo de saída.
    fprintf(arquivo_saida, "\n");

    va_end(args);
}

/**
 * Implementação da função que cria um novo rótulo.
 */
void novo_rotulo(char* rotulo) {
    // Usa sprintf para formatar a string do rótulo no buffer fornecido.
    // Ex: na primeira chamada, gera "L0"; na segunda, "L1"; e assim por diante.
    sprintf(rotulo, "L%d", contador_rotulo++);
}