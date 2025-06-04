#ifndef ANALEX
#define ANALEX
#define TAM_MAX_LEXEMA 31

enum TOKEN_CAT {ID=1, SN, CT_I, FIM_EXPR, FIM_ARQ};
/* Onde: ID: Identificador, SN: Sinal; CT_I: Constante numérica inteira*/

enum SINAIS {ATRIB=1, ADICAO, SUBTRACAO, MULTIPLICACAO, DIVISAO, ABRE_PAR, FECHA_PAR}; //Sinais válidos da linguagem

typedef 
    struct {
        enum TOKEN_CAT cat; // Deve receber uma das constantes de enum TOKEN_CAT
        union 
        {
            int codigo; // Para tokens das categorias SN
            int valInt; // Valor da constante inteira em tokens da cat. CT_I
            char lexema[TAM_MAX_LEXEMA]; // Cadeia de caracteres que corresponde ao nome do token da cat. ID 
        };
        
    } TOKEN; // Tipo Token

#endif
/* Contador de linhas do código fonte*/
int contLinha = 1;