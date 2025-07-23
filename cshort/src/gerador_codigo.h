#ifndef GERADOR_CODIGO_H
#define GERADOR_CODIGO_H

#include <stdio.h>

/**
 * @brief Inicializa o gerador de código.
 * Abre (ou cria) o arquivo de saída onde o código da Máquina de Pilha será escrito.
 * Esta função deve ser chamada uma única vez no início do programa.
 *
 * @param nome_arquivo O nome do arquivo a ser criado (ex: "saida_mp.txt").
 */
void inicializar_gerador(const char* nome_arquivo);

/**
 * @brief Finaliza o gerador de código.
 * Fecha o arquivo de saída, garantindo que todo o conteúdo seja salvo.
 * Esta função deve ser chamada uma única vez no final do programa.
 */
void finalizar_gerador();

/**
 * @brief Escreve uma instrução formatada no arquivo de saída.
 * Funciona de forma similar ao printf, mas direciona a saída para o arquivo de código gerado
 * e adiciona uma quebra de linha automaticamente.
 *
 * @param formato A string de formato da instrução (ex: "PUSH %d", "LABEL %s").
 * @param ... Argumentos variáveis para a string de formato.
 */
void gerar_codigo(const char* formato, ...);

/**
 * @brief Gera um novo rótulo único (L0, L1, L2, ...) para uso em desvios.
 * A cada chamada, um novo número é utilizado para garantir que os rótulos não se repitam.
 *
 * @param rotulo Um buffer de char (ex: char meu_rotulo[10];) para armazenar o rótulo gerado.
 */
void novo_rotulo(char* rotulo);

#endif // GERADOR_CODIGO_H