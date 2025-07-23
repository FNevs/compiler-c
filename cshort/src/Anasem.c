#include "Anasem.h"

extern void erro(char *msg);

/**
 * @brief Reporta um erro semântico formatado e encerra a compilação.
 * @param formato Uma string de formato no estilo printf.
 * @param ... Argumentos variáveis para a string de formato.
 */
void erro_semantico(const char* formato, ...) {
    char buffer_msg[1024];
    va_list args;

    va_start(args, formato);
    vsnprintf(buffer_msg, sizeof(buffer_msg), formato, args);
    va_end(args);

    erro(buffer_msg);
}

/**
 * @brief Converte um valor do enum TIPO_DADO para sua representação em string.
 * @param tipo O tipo de dado a ser convertido.
 * @return Uma string constante representando o tipo.
 */
const char* tipoParaString(TIPO_DADO tipo) {
    switch (tipo) {
        case TIPO_INT: return "int";
        case TIPO_CHAR: return "char";
        case TIPO_FLOAT: return "float";
        case TIPO_BOOL: return "bool";
        case TIPO_VAZIO: return "void";
        default: return "desconhecido";
    }
}

/**
 * @brief Verifica a compatibilidade de tipos em um comando de atribuição.
 * Permite algumas conversões implícitas (ex: int -> float).
 * @param tipo_lhs Tipo do lado esquerdo (variável).
 * @param tipo_rhs Tipo do lado direito (expressão).
 * @param nome_lhs Nome da variável do lado esquerdo, para mensagens de erro.
 */
void checaCompatibilidadeAtribuicao(TIPO_DADO tipo_lhs, TIPO_DADO tipo_rhs, const char* nome_lhs) {
    if (tipo_lhs == tipo_rhs) {
        return;
    }

    if ((tipo_lhs == TIPO_INT || tipo_lhs == TIPO_CHAR || tipo_lhs == TIPO_BOOL) &&
        (tipo_rhs == TIPO_INT || tipo_rhs == TIPO_CHAR || tipo_rhs == TIPO_BOOL)) {
        return; // Conversão permitida
    }
    if (tipo_lhs == TIPO_FLOAT && tipo_rhs == TIPO_INT) {
        return; // Conversão permitida
    }

    erro_semantico("Tipos incompativeis na atribuicao. Nao e possivel atribuir o tipo '%s' a variavel '%s' que e do tipo '%s'.",
                     tipoParaString(tipo_rhs), nome_lhs, tipoParaString(tipo_lhs));
}

/**
 * @brief Verifica a compatibilidade de tipos para operadores aritméticos (+, -, *, /)
 * e retorna o tipo resultante da operação (com promoção de tipo).
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return O tipo de dado resultante (ex: TIPO_FLOAT se um dos operandos for float).
 */
TIPO_DADO getTipoResultanteAritmetico(TIPO_DADO op1, TIPO_DADO op2) {
    
    if ((op1 == TIPO_INT || op1 == TIPO_CHAR || op1 == TIPO_FLOAT) &&
        (op2 == TIPO_INT || op2 == TIPO_CHAR || op2 == TIPO_FLOAT)) {
        
        if (op1 == TIPO_FLOAT || op2 == TIPO_FLOAT) {
            return TIPO_FLOAT;
        }

        if (op1 == TIPO_INT || op2 == TIPO_INT) {
            return TIPO_INT;
        }

        return TIPO_CHAR;
    }
    
    erro_semantico("Operadores aritmeticos exigem tipos numericos (int, char, float), mas foram recebidos '%s' e '%s'.",
                   tipoParaString(op1), tipoParaString(op2));
    return TIPO_DESCONHECIDO; 
}

/**
 * @brief Verifica a compatibilidade para operadores relacionais (==, !=, <, >) e retorna o tipo do resultado.
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return Sempre TIPO_BOOL, pois o resultado de uma comparação é um booleano.
 */
TIPO_DADO getTipoResultanteRelacional(TIPO_DADO op1, TIPO_DADO op2) {
    if ((op1 == TIPO_INT || op1 == TIPO_CHAR || op1 == TIPO_FLOAT) &&
        (op2 == TIPO_INT || op2 == TIPO_CHAR || op2 == TIPO_FLOAT)) {
        return TIPO_BOOL; // O resultado de uma relação é sempre booleano.
    }
    
    erro_semantico("Operadores relacionais exigem tipos numericos (int, char, float), mas foram recebidos '%s' e '%s'.",
                   tipoParaString(op1), tipoParaString(op2));
    return TIPO_DESCONHECIDO; 
}


/**
 * @brief Verifica a compatibilidade para operadores lógicos (&&, ||) e retorna o tipo do resultado.
 * @param op1 Tipo do primeiro operando.
 * @param op2 Tipo do segundo operando.
 * @return Sempre TIPO_BOOL.
 */
TIPO_DADO getTipoResultanteLogico(TIPO_DADO op1, TIPO_DADO op2) {
    if ((op1 == TIPO_BOOL || op1 == TIPO_INT) && (op2 == TIPO_BOOL || op2 == TIPO_INT)) {
        return TIPO_BOOL; // O resultado de uma operação lógica é sempre booleano.
    }

    erro_semantico("Operadores logicos (&&, ||) exigem tipos booleano ou int, mas foram recebidos '%s' e '%s'.",
                   tipoParaString(op1), tipoParaString(op2));
    return TIPO_DESCONHECIDO; // Não deve ser alcançado
}


/**
 * @brief Verifica se a expressão em um comando condicional (if, while) é válida.
 * @param comando O nome do comando ("if", "while") para a mensagem de erro.
 * @param tipo_expr O tipo da expressão avaliada.
 */
void checaCondicao(const char* comando, TIPO_DADO tipo_expr) {
    if (tipo_expr != TIPO_BOOL && tipo_expr != TIPO_INT) {
        erro_semantico("A expressao condicional em um comando '%s' deve ser do tipo booleano ou int, mas foi recebido o tipo '%s'.",
                       comando, tipoParaString(tipo_expr));
    }
}

/**
 * @brief Verifica se o tipo de uma expressão de retorno é compatível com o tipo de retorno declarado da função.
 * @param func_id O identificador da função, obtido da tabela de símbolos.
 * @param tipo_expr_retornada O tipo da expressão no comando 'return'.
 * @param tem_expr Verdadeiro se o comando 'return' tem uma expressão, falso caso contrário (ex: return;).
 */
void checaRetornoFuncao(IDENTIFICADOR func_id, TIPO_DADO tipo_expr_retornada, bool tem_expr) {
    TIPO_DADO tipo_retorno_declarado = func_id.tipo;

    if (tipo_retorno_declarado == TIPO_VAZIO) {
        if (tem_expr) {
            erro_semantico("A funcao '%s' e do tipo 'void' e nao pode retornar um valor.", func_id.nome);
        }
    } else {
        if (!tem_expr) {
            erro_semantico("A funcao '%s' deve retornar um valor do tipo '%s', mas nenhum valor foi retornado.",
                           func_id.nome, tipoParaString(tipo_retorno_declarado));
        }
        
        checaCompatibilidadeAtribuicao(tipo_retorno_declarado, tipo_expr_retornada, func_id.nome);
    }
}