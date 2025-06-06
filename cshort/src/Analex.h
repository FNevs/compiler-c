#ifndef _ANALEX_
#define _ANALEX_
#define TAMANHO_MAX_LEXEMA 50
#define TAMANHO_MAX_STRING 100
#define TAMANHO_NUM 20 

#include <stdio.h>
#include <stdbool.h>

enum TOKEN_CAT {
    ID = 1,         // Identificador
    PR,             // Palavra Reservada
    SN,             // Sinal
    CT_INT,         // Constante Inteira
    CT_REAL,        // Constante Real
    CT_STRING,      // Constante String
    CT_CHAR,        // Constante Caractere
    FIM_ARQ,        // Fim de Arquivo
};

enum SINAIS {
    ATRIB = 1,      // Atribuição
    ADICAO,
    SUBTRACAO,
    MULTIPLICACAO,
    DIVISAO,
    ABRE_PAREN,
    FECHA_PAREN,
    ABRE_COLCH,
    FECHA_COLCH,
    ABRE_CHAVES,
    FECHA_CHAVES,
    VIRGULA,
    PONTO_VIRGULA,
    MENOR_QUE,
    MAIOR_QUE,
    MENOR_IGUAL,
    MAIOR_IGUAL,
    IGUALDADE,
    DIFERENTE,
    NOT,
    AND,
    OR,
    E_COMERCIAL,
};

enum PALAVRAS_RESERVADAS {
    INT, 
    CHAR,
    FLOAT,
    BOOL,
    VOID,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
};
typedef 
    struct {
        enum TOKEN_CAT cat; // Categoria do token, recebe um valor do tipo enum TOKEN_CAT
        union { // Para armazenar diferentes tipos de dados
            int codigo; // Código do sinal ou palavra reservada
            char lexema[TAMANHO_MAX_LEXEMA]; // Cadeia de caracteres que corresponde ao token cat. ID
            char string[TAMANHO_MAX_STRING]; // String para constantes do tipo string no token cat. STRINGCON
            char caractere; // Caractere para constantes do tipo char, no token cat. CHARCON
            int valor_int; // Valor inteiro para constantes do tipo int, no token cat. INTCON
            float valor_real; // Valor real para constantes do tipo float, no token cat. REALCON
        };
    } TOKEN; // Definição do tipo TOKEN

/* Variaveis globais*/
extern int contLinha; // Contador de linhas
extern TOKEN t;
extern FILE *fd; // Arquivo de entrada
extern char *palavras_reservas[];

/* Assinaturas de funcoes*/
TOKEN Analex(FILE *fd);

#endif