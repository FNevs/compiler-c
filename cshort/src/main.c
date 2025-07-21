#include <stdio.h>
#include <stdlib.h>
#include "Anasint.h"

// Exemplo de compilação: gcc main.c Anasint.c Analex.c -o meu_compilador
TABELA_IDENTIFICADOR tabelaIdentificadores;

int main(int argc, char *argv[]) {
    // 1. Verificar se o nome do arquivo foi passado como argumento
    if (argc < 2) {
        fprintf(stderr, "Modo de uso: %s <caminho_para_o_arquivo.cshort>\n", argv[0]);
        return 1; // Retorna código de erro
    }

    // 2. Abrir o arquivo de código-fonte para leitura
    // A variável 'fd' é global e declarada como 'extern' nos headers
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        fprintf(stderr, "ERRO: Não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    // 3. Chamar a função inicial do Analisador Sintático
    // A função Prog() irá consumir os tokens, analisar a sintaxe e,
    // em caso de erro, a função erro() (definida em Analex.c)
    // irá encerrar o programa.
    printf("Iniciando análise sintática do arquivo: %s\n\n", argv[1]);

    Prog(); // Ponto de entrada do parser

    // 4. Se o programa chegou até aqui, a análise foi bem-sucedida
    printf("\nAnálise sintática concluída com sucesso! O arquivo '%s' é sintaticamente válido.\n", argv[1]);

    // 5. Fechar o arquivo e terminar
    fclose(fd);
    return 0; // Sucesso
}