#ifndef GERADOR_CODIGO_H
#define GERADOR_CODIGO_H

#include <stdio.h>

void inicializar_gerador(const char* nome_arquivo);

void finalizar_gerador();

void gerar_codigo(const char* formato, ...);

void novo_rotulo(char* rotulo);

#endif // GERADOR_CODIGO_H