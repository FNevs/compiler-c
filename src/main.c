#include <stdio.h>
#include <stdlib.h>
#include "Anasint.h"
#include "GeradorCodigo.h" 

TABELA_IDENTIFICADOR tabelaIdentificadores; 

int main(int argc, char *argv[]) {
    // Verificar se o nome do arquivo foi passado como argumento
    if (argc < 2) {
        fprintf(stderr, "Modo de uso: %s <caminho_para_o_arquivo.cshort>\n", argv[0]);
        return 1;
    }

    // Abrir o arquivo de código-fonte para leitura
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        fprintf(stderr, "ERRO: Não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    // Inicializa o gerador de código.
    InicializarGerador("saida.mp");

    // Chamar a função inicial do Analisador Sintático
    printf("Iniciando análise sintática do arquivo: %s\n\n", argv[1]);

    Prog(); // Ponto de entrada do parser

    FinalizarGerador();

    // Atualiza a mensagem de sucesso
    printf("\nAnálise concluída! Código para a Máquina de Pilha salvo em 'saida.mp'.\n");


    // Fechar o arquivo e terminar
    fclose(fd);
    return 0; // Sucesso
}