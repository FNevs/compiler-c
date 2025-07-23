#ifndef GERADOR_CODIGO_H
#define GERADOR_CODIGO_H

#include <stdio.h>
#include <stdarg.h> 
#include <stdlib.h> 


/**
 * @brief Função que abre o arquivo de saída.
 * @param nome_arquivo Nome do arquivo de saída
 */
void InicializarGerador(const char* nome_arquivo);


/**
 * @brief Função que fecha o arquivo de saída.
 */
void FinalizarGerador();



/**
 * @brief Função principal de geração de código.
 * @param f Próxima instrução para a geração de código.
 */
void GerarCodigo(const char* f, ...);


#endif // GERADOR_CODIGO_H