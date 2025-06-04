#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <string.h> 
#include <stdbool.h> 
#include "Analex_calc.h" 

#define TAM_LEXEMA 50 
#define TAM_NUM 20 

void error(char msg[]) { 
    printf("%s\n", msg); 
    exit(1); 
} 

TOKEN AnaLex(FILE *fd) { 
    int estado; 
    char lexema[TAM_LEXEMA] = ""; 
    int tamL = 0; 
    char digitos[TAM_NUM] = ""; 
    int tamD = 0; 
    TOKEN t; 

    estado = 0; 
    while (true) { 
        char c = fgetc(fd); 
        switch (estado) { 
            case 0: if (c == ' ' || c == '\t') estado = 0; 
                    else if (c >= 'a' && c <= 'z') {  // Início de identificador - inicializa lexema 
                        estado = 1; 
                        lexema[tamL] = c;  
                        lexema[++tamL] = '\0'; 
                    } 
                    else if (c >= '1' && c <= '9') {  // Início de constante inteira - inicializa dígitos 
                        estado = 10; 
                        digitos[tamD] = c;  
                        digitos[++tamD] = '\0';     
                    } 
                    else if (c == '+') {   // Sinal de adição - monta e devolve token 
                        estado = 3; 
                        t.cat = SN; 
                        t.codigo = ADICAO; 
                        return t; 
                    } 
                    else if (c == '-') {   // Sinal de subtração - monta e devolve token 
                        estado = 4; 
                        t.cat = SN; 
                        t.codigo = SUBTRACAO; 
                        return t; 
                    } 
                    else if (c == '*') {   // Sinal de multiplicação - monta e devolve token 
                        estado = 6; 
                        t.cat = SN; 
                        t.codigo = MULTIPLICACAO; 
                        return t; 
                    } 
                    else if (c == '/') {   // Sinal de divisão - monta e devolve token 
                        estado = 5; 
                        t.cat = SN; 
                        t.codigo = DIVISAO; 
                        return t; 
                    } 
                    else if (c == '=') {   // Sinal de atribuição - monta e devolve token 
                        estado = 7; 
                        t.cat = SN; 
                        t.codigo = ATRIB; 
                        return t; 
                    } 
                    else if (c == '(') {   // Sinal de abre parênteses - monta e devolve token 
                        estado = 8; 
                        t.cat = SN; 
                        t.codigo = ABRE_PAR; 
                        return t; 
                    } 
                    else if (c == ')') {   // Sinal de fecha parênteses - monta e devolve token 
                        estado = 9; 
                        t.cat = SN; 
                        t.codigo = FECHA_PAR; 
                        return t; 
                    } 
                    else if (c == '\n') { 
                        estado = 0; 
                        t.cat = FIM_EXPR;   // Fim de linha (ou expressão) encontrado 
                        contLinha++; 
                        return t; 
                    } 
                    else if (c == EOF) {   // Fim do arquivo fonte encontrado 
                         t.cat = FIM_ARQ; 
                         return t; 
                    } 
                    else  
                        error("Caracter invalido na expressao!");   // Sem transição válida no AFD 
                    break; 
            case 1: if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_')) {   
                        estado = 1; 
                        lexema[tamL] = c;     // Acumula caracteres lidos em lexema 
                        lexema[++tamL] = '\0'; 
                    } 
                    else {                     // Transição OUTRO* do estado 1 do AFD 
                        estado = 2;            // Monta token identificador e retorna 
                        ungetc(c, fd); 
                        t.cat = ID; 
                        strcpy(t.lexema, lexema); 
                        return t; 
                    } 
                    break; 
            case 10:if (c >= '0' && c <= '9') { 
                        estado = 10; 
                        digitos[tamD] = c;       // Acumula dígitos lidos na variável digitos 
                        digitos[++tamD] = '\0'; 
                    } 
                    else {                       // Transição OUTRO* do estado 10 do AFD 
                        estado = 11;             // Monta token constante inteira e retorna 
                        ungetc(c, fd); 
                        t.cat = CT_I; 
                        t.valInt = atoi(digitos); 
                        return t; 
                    } 
        }                 
    } 
} 

int main() { 
    FILE *fd; 
    TOKEN tk; 

    if ((fd=fopen("expressao.dat", "r")) == NULL) 
        error("Arquivo de entrada da expressao nao encontrado!"); 
    
    printf("LINHA %d: ", contLinha); 
    
    while (1) {    // Laço infinito para processar a expressão até o fim de arquivo 
        tk = AnaLex(fd); 
        switch (tk.cat) { 
            case ID: printf("<ID, %s> ", tk.lexema); 
                     break; 
            case SN: switch (tk.codigo) { 
                     case ADICAO: printf("<SN, ADICAO> "); 
                     break; 
                     case SUBTRACAO: printf("<SN, SUBTRACAO> "); 
                     break; 
                     case MULTIPLICACAO: printf("<SN, MULTIPLICACAO> "); 
                     break; 
                     case DIVISAO: printf("<SN, DIVISAO> "); 
                     break; 
                     case ATRIB: printf("<SN, ATRIBUICAO> "); 
                     break; 
                     case ABRE_PAR: printf("<SN, ABRE_PARENTESES> "); 
                     break; 
                     case FECHA_PAR: printf("<SN, FECHA_PARENTESES> "); 
                     break; 
                     } 
                     break; 
            case CT_I: printf("<CT_I, %d> ", tk.valInt); 
                     break; 
            case FIM_EXPR: printf("<FIM_EXPR, %d>\n", 0); 
                         printf("LINHA %d: ", contLinha); 
                         break;                  
            case FIM_ARQ: printf(" <Fim do arquivo encontrado>\n"); 
        } 
        if (tk.cat == FIM_ARQ) break; 
    } 

    fclose(fd); 
    return 0; 
} 