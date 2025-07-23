#include "GeradorCodigo.h"

static FILE* output;

void InicializarGerador(const char* nome_arquivo) {
    output = fopen(nome_arquivo, "w");

    if (output == NULL) 
    {
        printf("Não conseguiu abrir o output file '%s'.\n", nome_arquivo);
        exit(1);
    }
}

void FinalizarGerador() {
    if (output != NULL) 
    {
        fclose(output);
        output = NULL; 
    }
}

void GerarCodigo(const char* f, ...) {
    if (output == NULL) 
    {
        return;
    }

    va_list args;
    va_start(args, f);
    vfprintf(output, f, args);
    fprintf(output, "\n");
    va_end(args);
}
