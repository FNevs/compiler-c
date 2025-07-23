#include "Analex.h"
#include "TabIdent.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef ANASEM_H
#define ANASEM_H

/**
 * @brief Reporta um erro semântico formatado e encerra a compilação.
 * @param formato Uma string de formato no estilo printf.
 * @param ... Argumentos variáveis para a string de formato.
 */
void erro_semantico(const char* formato, ...);

/**
 * @brief Converte um valor do enum TIPO_DADO para sua representação em string.
 * @param tipo O tipo de dado a ser convertido.
 * @return Uma string constante representando o tipo.
 */
const char* tipoParaString(TIPO_DADO tipo);

/**
 * @brief Verifica a compatibilidade de tipos em um comando de atribuição.
 * Permite algumas conversões implícitas (ex: int -> float).
 * @param tipo_lhs Tipo do lado esquerdo (variável).
 * @param tipo_rhs Tipo do lado direito (expressão).
 * @param nome_lhs Nome da variável do lado esquerdo, para mensagens de erro.
 */
void checaCompatibilidadeAtribuicao(TIPO_DADO tipo_lhs, TIPO_DADO tipo_rhs, const char* nome_lhs);

/**
 * @brief Verifica a compatibilidade de tipos para operadores aritméticos (+, -, *, /)
 * e retorna o tipo resultante da operação (com promoção de tipo).
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return O tipo de dado resultante (ex: TIPO_FLOAT se um dos operandos for float).
 */
TIPO_DADO getTipoResultanteAritmetico(TIPO_DADO op1, TIPO_DADO op2);

/**
 * @brief Verifica a compatibilidade para operadores relacionais (==, !=, <, >) e retorna o tipo do resultado.
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return Sempre TIPO_BOOL, pois o resultado de uma comparação é um booleano.
 */
TIPO_DADO getTipoResultanteRelacional(TIPO_DADO op1, TIPO_DADO op2);

/**
 * @brief Verifica a compatibilidade para operadores lógicos (&&, ||) e retorna o tipo do resultado.
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return Sempre TIPO_BOOL.
 */
TIPO_DADO getTipoResultanteLogico(TIPO_DADO op1, TIPO_DADO op2);

/**
 * @brief Verifica se a expressão em um comando condicional (if, while) é válida.
 * @param comando O nome do comando ("if", "while") para a mensagem de erro.
 * @param tipo_expr O tipo da expressão avaliada.
 */
void checaCondicao(const char* comando, TIPO_DADO tipo_expr);

/**
 * @brief Verifica se o tipo de uma expressão de retorno é compatível com o tipo de retorno declarado da função.
 * @param func_id O identificador da função, obtido da tabela de símbolos.
 * @param tipo_expr_retornada O tipo da expressão no comando 'return'.
 * @param tem_expr Verdadeiro se o comando 'return' tem uma expressão, falso caso contrário (ex: return;).
 */
void checaRetornoFuncao(IDENTIFICADOR func_id, TIPO_DADO tipo_expr_retornada, bool tem_expr);

#endif // ANASEM_H