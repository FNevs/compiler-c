#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Analex.h"

// Definições Globais e Variáveis
int contLinha = 1;
TOKEN t;
FILE *fd;

// Definição do array de palavras reservadas
char *palavras_reservadas[] = {
    "int", "char", "float", "bool", "void",
    "if", "else", "while", "for", "return"
};
const int NUM_PALAVRAS_RESERVADAS = sizeof(palavras_reservadas) / sizeof(palavras_reservadas[0]);

// Constantes para os caracteres de escape 
#define BARRA_N '\n'
#define BARRA_0 '\0'

// Função de Erro
void erro(char *msg) {
    fprintf(stderr, "ERRO LÉXICO (Linha %d): %s\n", contLinha, msg);
    exit(1);
}

// Função Principal do Analisador Léxico
TOKEN Analex(FILE *arquivo_entrada) {
    if (fd == NULL) {
        fd = arquivo_entrada;
    }

    int estado = 0;
    // Buffers separados
    char lexema[TAMANHO_MAX_LEXEMA];
    char digitos[TAMANHO_NUM];
    char string[TAMANHO_MAX_STRING];
    int tamL = 0, tamD = 0, tamS = 0;

    while (true) {
        char c = fgetc(fd);

        switch (estado) {
            // Estado q0: Inicial / Distribuidor
            case 0:
                tamL = 0; tamD = 0; tamS = 0; // Zera os contadores de tamanho
                if (c == ' ' || c == '\t') { estado = 0; }
                else if (c == '\n') { contLinha++; estado = 0; }
                else if (isalpha(c) || c == '_') {
                    lexema[tamL++] = c;
                    estado = 1;
                }
                else if (isdigit(c)) {
                    digitos[tamD++] = c;
                    estado = 3;
                }
                else if (c == '\"') {
                    estado = 9;
                }
                else if (c == '\'') {
                    estado = 8; // Inicia o estado para CHARCON
                }
                else if (c == '/') {
                    estado = 16;
                }
                else if (c == '<') {
                    estado = 10;
                }
                else if (c == '>') {
                    estado = 11;
                }
                else if (c == '=') {
                    estado = 12;
                }
                else if (c == '!') {
                    estado = 13;
                }
                else if (c == '&') {
                    estado = 14;
                }
                else if (c == '|') {
                    estado = 15;
                }
                else if (c == EOF) {
                    estado = 49;
                    t.cat = FIM_ARQ;
                    return t;
                }
                else if (c == '*') {
                    estado = 41; //41
                    t.cat = SN;
                    t.codigo = MULTIPLICACAO;
                    return t;
                }
                else if (c == '-') {
                    estado = 39; //39
                    t.cat = SN;
                    t.codigo = SUBTRACAO;
                    return t;
                }
                else if (c == '+') {
                    estado = 40; //40
                    t.cat = SN;
                    t.codigo = ADICAO;
                    return t;
                }
                else if (c == '(') {
                    estado = 42; //42
                    t.cat = SN;
                    t.codigo = ABRE_PAREN;
                    return t;
                }
                else if (c == ')') {
                    estado = 43; //43
                    t.cat = SN;
                    t.codigo = FECHA_PAREN;
                    return t;
                }
                else if (c == '[') {
                    estado = 44; //44
                    t.cat = SN;
                    t.codigo = ABRE_COLCH;
                    return t;
                }
                else if (c == ']') {
                    estado = 45; //45
                    t.cat = SN;
                    t.codigo = FECHA_COLCH;
                    return t;
                }
                else if (c == '{') {
                    estado = 46; //46
                    t.cat = SN;
                    t.codigo = ABRE_CHAVES;
                    return t;
                }
                else if (c == '}') {
                    estado = 47; //47
                    t.cat = SN;
                    t.codigo = FECHA_CHAVES;
                    return t;
                }
                else if (c == ';') {
                    estado = 50;
                    t.cat = SN;
                    t.codigo = PONTO_VIRGULA;
                    return t;
                }
                else if (c == ',') {
                    estado = 48;
                    t.cat = SN;
                    t.codigo = VIRGULA;
                    return t;
                }
                else {
                    erro("Caractere inválido.");
                }
                break;

            // case 1 com a atribuição para o estado q2
            case 1:
                if (isalnum(c) || c == '_') {
                    lexema[tamL++] = c;
                    estado = 1; // Permanece no estado q1
                } else {
                    ungetc(c, fd);
                    lexema[tamL] = '\0';
                    
                    estado = 2; // Atribuição para o estado final q2 do AFD

                    // Checa se é Palavra Reservada
                    bool eh_pr = false;
                    for (int i = 0; i < NUM_PALAVRAS_RESERVADAS; i++) {
                        if (strcmp(lexema, palavras_reservadas[i]) == 0) {
                            t.cat = PR; t.codigo = i; eh_pr = true;
                            return t;
                        }
                    }
                    
                    // Se não for PR, é um ID
                    t.cat = ID; 
                    strcpy(t.lexema, lexema);
                    return t;
                }
                break;

            // Estados q3, q5, q6: Números
            case 3:
                if (isdigit(c)) { digitos[tamD++] = c; estado = 3; }
                else if (c == '.') { digitos[tamD++] = c; estado = 5; } // Vai para q5
                else {
                    ungetc(c, fd);
                    digitos[tamD] = '\0';
                    estado = 4; // Estado final q4 do AFD
                    t.cat = CT_INT; t.valor_int = atoi(digitos);
                    return t;
                }
                break;
            case 5:
                if (isdigit(c)) { digitos[tamD++] = c; estado = 6; } // Vai para q6
                else { erro("Constante real malformada."); }
                break;
            case 6:
                if (isdigit(c)) { digitos[tamD++] = c; estado = 6; }
                else {
                    ungetc(c, fd);
                    digitos[tamD] = '\0';
                    estado = 7; // Estado final q7 do AFD
                    t.cat = CT_REAL; t.valor_real = atof(digitos);
                    return t;
                }
                break;
            
            // Estados q8, q32, q33, q34, q35, q36: Lógica para CHARCON
            case 8: // Viu '\''
                if (c == '\\') {
                    estado = 33;
                }
                else if (c != '\'') {
                    lexema[0] = c;
                    estado = 32;
                }
                else {
                    erro("Constante caractere vazia.");
                }
                break;

            case 32: // Viu o caractere comum
                if (c == '\'') {
                    estado = 34;
                    t.cat = CT_CHAR;
                    t.caractere = lexema[0];
                    return t;
                }
                else {
                    erro("Constante caractere com mais de um caractere.");
                }
                break;

            case 33: // Viu '\'
                if (c == 'n') {
                    lexema[0] = BARRA_N;
                    estado = 35;
                }
                else if (c == '0') {
                    lexema[0] = BARRA_0;
                    estado = 35;
                }
                else {
                    erro("Sequência de escape inválida.");
                }
                break;

            case 35: // Viu '\n' ou '\0'
                if (c == '\'') {
                    estado = 36;
                    t.cat = CT_CHAR;
                    t.caractere = lexema[0];
                    return t;
                }
                else {
                    erro("Constante caractere (escape) malformada.");
                }
                break;

            // Estados q9, q31: Lógica para STRINGCON
            case 9:
                if (c == '\"') {
                    string[tamS] = '\0';
                    estado = 31; // Estado final q31 do AFD
                    t.cat = CT_STRING;
                    strcpy(t.string, string);
                    return t;
                }
                else if (c == '\n' || c == EOF) {
                    erro("String não terminada.");
                }
                else {
                    string[tamS++] = c;
                    estado = 9;
                }
                break;

            // Estados de operadores de múltiplos caracteres
            case 10: // Viu '<'
                if (c == '=') {
                    estado = 17;
                    t.cat = SN;
                    t.codigo = MENOR_IGUAL;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    estado = 18;
                    t.cat = SN;
                    t.codigo = MENOR_QUE;
                    return t;
                }

            case 11: // Viu '>'
                if (c == '=') {
                    estado = 19;
                    t.cat = SN;
                    t.codigo = MAIOR_IGUAL;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    estado = 20;
                    t.cat = SN;
                    t.codigo = MAIOR_QUE;
                    return t;
                }

            case 12: // Viu '='
                if (c == '=') {
                    estado = 21;
                    t.cat = SN;
                    t.codigo = IGUALDADE;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    estado = 22;
                    t.cat = SN;
                    t.codigo = ATRIB;
                    return t;
                }

            case 13: // Viu '!'
                if (c == '=') {
                    estado = 23;
                    t.cat = SN;
                    t.codigo = DIFERENTE;
                    return t;
                }
                else {
                    ungetc(c, fd); // Retorna o caractere para o fluxo
                    estado = 24;
                    t.cat = SN;
                    t.codigo = NOT;
                    return t;
                }

            case 14: // Viu '&'
                if (c == '&') {
                    estado = 25;
                    t.cat = SN;
                    t.codigo = AND;
                    return t;
                }
                else {
                    ungetc(c, fd);
                    estado = 26;
                    t.cat = SN;
                    t.codigo = E_COMERCIAL;
                    return t;
                }

            case 15: // Viu '|'
                if (c == '|') {
                    estado = 27;
                    t.cat = SN;
                    t.codigo = OR;
                    return t;
                }
                else {
                    erro("Operador '|' inválido, esperado '||'.");
                }
                break; // break necessário para os casos que não retornam

            // Estados de Comentário e Divisão
            case 16: // Viu '/'
                if (c == '*') {
                    estado = 28;
                }
                else {
                    ungetc(c, fd);
                    estado = 29;
                    t.cat = SN;
                    t.codigo = DIVISAO;
                    return t;
                }
                break;

            case 28: // Dentro de comentário
                if (c == '*') {
                    estado = 30;
                }
                else if (c == EOF) {
                    erro("Comentário não terminado.");
                }
                else {
                    estado = 28;
                }
                break;

            case 30: // Viu '*' em comentário
                if (c == '/') {
                    estado = 0;
                }
                else if (c == EOF) {
                    erro("Comentário não terminado.");
                }
                else if (c != '*') {
                    estado = 28;
                }
                else {
                    estado = 30; // Viu '***'
                }
                break;

        } // Fim do switch
    } // Fim do while
}

// =================================================================================
// Função Auxiliar para imprimir Sinais (Operadores/Pontuação)
const char* getSinalStr(int codigo_sinal) {
    switch(codigo_sinal) {
        case ATRIB: return "=";
        case ADICAO: return "+";
        case SUBTRACAO: return "-";
        case MULTIPLICACAO: return "*";
        case DIVISAO: return "/";
        case ABRE_PAREN: return "(";
        case FECHA_PAREN: return ")";
        case ABRE_COLCH: return "[";
        case FECHA_COLCH: return "]";
        case ABRE_CHAVES: return "{";
        case FECHA_CHAVES: return "}";
        case VIRGULA: return ",";
        case PONTO_VIRGULA: return ";";
        case MENOR_QUE: return "<";
        case MAIOR_QUE: return ">";
        case MENOR_IGUAL: return "<=";
        case MAIOR_IGUAL: return ">=";
        case IGUALDADE: return "==";
        case DIFERENTE: return "!=";
        case NOT: return "!";
        case AND: return "&&";
        case OR: return "||";
        case E_COMERCIAL: return "&";
        default: return "SINAL_DESCONHECIDO";
    }
}

// Função main para Testes 
int main(int argc, char *argv[]) {
    // 1. Validação dos Argumentos (melhor que ter o nome do arquivo fixo)
    if (argc < 2) {
        fprintf(stderr, "Modo de uso: %s <nome_do_arquivo.cshort>\n", argv[0]);
        return 1;
    }

    // 2. Abertura do Arquivo Fonte
    if ((fd = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "ERRO: Arquivo de entrada '%s' não encontrado!\n", argv[1]);
        return 1;
    }

    printf("--- Iniciando Análise do Arquivo: %s ---\n", argv[1]);

    // 3. Laço Principal de Análise
    while (true) {
        Analex(fd); // A função preenche a variável global 't'

        switch (t.cat) {
            case ID:
                printf("Linha %-3d: <ID, %s>\n", contLinha, t.lexema);
                break;
            case PR:
                printf("Linha %-3d: <PR, %s>\n", contLinha, palavras_reservadas[t.codigo]);
                break;
            case CT_INT:
                printf("Linha %-3d: <CT_INT, %d>\n", contLinha, t.valor_int);
                break;
            case CT_REAL:
                printf("Linha %-3d: <CT_REAL, %f>\n", contLinha, t.valor_real);
                break;
            case CT_STRING:
                printf("Linha %-3d: <CT_STRING, \"%s\">\n", contLinha, t.string);
                break;
            case CT_CHAR:
                if (t.caractere == '\n') {
                    printf("Linha %-3d: <CT_CHAR, '\\n'>\n", contLinha);
                } else if (t.caractere == '\0') {
                    printf("Linha %-3d: <CT_CHAR, '\\0'>\n", contLinha);
                } else {
                    printf("Linha %-3d: <CT_CHAR, '%c'>\n", contLinha, t.caractere);
                }
                break;
            case SN:
                printf("Linha %-3d: <SN, %s>\n", contLinha, getSinalStr(t.codigo));
                break;
            case FIM_ARQ:
                printf("--- Fim de Arquivo ---\n");
                fclose(fd);
                return 0; // Encerra o programa
        }
    }
}