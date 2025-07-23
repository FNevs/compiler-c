/*
  Este arquivo irá conter a implementação do Analisador Sintático
  Descendente Recursivo (ASDR). Cada função corresponde a um
  não-terminal da gramática abaixo.

======================================================================
Regra de Produção da Gramática Cshort - Sem Backtracking 
======================================================================

<prog>        ::= { decl ( ';' | '{' corpo_func '}' ) }

<decl>        ::= <tipo> <decl_var> { ',' <decl_var> }
              | <tipo> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }
              | <void> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }

<decl_var>    ::= <id> [ '[' <intcon> ']' ]

<tipo>        ::= 'char' | 'int' | 'float' | 'bool'

<tipos_param> ::= <void>
              | <tipo> [ '&' ] <id> [ '[' ']' ] { ',' <tipo> [ '&' ] <id> [ '[' ']' ] }

<corpo_func>  ::= { tipo decl_var { ',' decl_var} ';' } { cmd }

// Regras de Comandos
<cmd>         ::= 'if' '(' expr ')' cmd [ 'else' cmd ]
              | 'while' '(' expr ')' cmd
              | 'for' '(' [atrib] ';' [expr] ';' [atrib] ')' cmd
              | 'return' [expr] ';'
              | '{' {cmd} '}'
              | id cmd_cont

<cmd_cont>    ::= '(' [expr {',' expr}] ')' ';'
              | ['[' expr ']'] '=' expr ';'


<atrib>       ::= <id> [ '[' <expr> ']' ] '=' <expr>

<expr>        ::= <expr_simp> [ <op_rel> <expr_simp> ]

<expr_simp>   ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

<termo>       ::= <fator> { ( '*' | '/' | '&&' ) <fator> }

// Regras de Fator (fatorada)
fator         ::= intcon
              | realcon
              | charcon
              | '(' expr ')'
              | '!' fator
              | id fator_cont

fator_cont    ::= '[' expr ']'
              | '(' [expr {',' expr}] ')'
              | ε

<op_rel>      ::= '==' | '!=' | '<=' | '<' | '>=' | '>'

*/

#include "Anasint.h" 

ESCOPO escopo = GLOBAL;
IDENTIFICADOR funcao_atual;
int offset_local_atual = 0;


void processa_lista_declaracao_vars(TIPO_DADO tipo) {
    TOKEN idToken;

    if (t.cat != ID) {
        erro("Identificador esperado na declaração de variável.");
    }
    idToken = t;

    // Checa por redeclaração no escopo atual
    if (BuscaTabelaIDMesmoEscopo(idToken.lexema, escopo) != -1) {
        char msg[100];
        sprintf(msg, "Redeclaração do identificador '%s'", idToken.lexema);
        erro(msg);
    }

    t = Analex(fd); // Consome o ID

    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        // É um array
        t = Analex(fd); // Consome '['
        if (t.cat != CT_INT) erro("Tamanho do array deve ser uma constante inteira.");
        int tamArray = t.valor_int;
        InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipo, false, true, tamArray);
        t = Analex(fd); // Consome o número
        if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' para fechar dimensão do array.");
        t = Analex(fd); // Consome ']'
    } else {
        // É uma variável simples
        InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipo, false, false, 0);
    }
}


TIPO_DADO getTokenTipo(int cod) {
    switch(cod) {
        case INT: return TIPO_INT;
        case CHAR: return TIPO_CHAR;
        case FLOAT: return TIPO_FLOAT;
        case BOOL: return TIPO_BOOL;
        default: return TIPO_VAZIO;
    }
}

int contRotulo = 0;
int novoRotulo() {
    return contRotulo++;
}


void Prog() {
    DECL_SINALIZADOR declFlag;
 
    printf("INIP\n");
    t = Analex(fd);
 
    while (t.cat != FIM_ARQ) {
        declFlag = Decl();
        if (t.cat == SN && t.codigo == PONTO_VIRGULA) {
            t = Analex(fd); 
        }
        else if (t.cat == SN && t.codigo == ABRE_CHAVES) {

            funcao_atual = tabelaIdentificadores.identificadores[tabelaIdentificadores.tamTabela - 1];

            // Gera o cabeçalho da função
            printf("LABEL %s\n", funcao_atual.nome);
            printf("INIPR 1\n");

            t = Analex(fd); // Consome o '{'
            int locais_count = corpo_func();
            
            if (!(t.cat == SN && t.codigo == FECHA_CHAVES)) {
                erro("Esperado '}' para fechar o corpo da função");
            }
            
            if (locais_count > 0) {
                printf("DMEM %d\n", locais_count);
            }
            printf("RET 1, 0\n");

            t = Analex(fd); // Consome o '}'
        }
        else {
            erro("Esperado ';' ou '{' após a declaração");
        }
    }
    
    int main_idx = BuscaTabelaID("main");
    if (main_idx == -1) {
        erro("Função 'main' não definida no programa.");
    }
    
    printf("CALL main\n");
    printf("HALT\n");
}


void trata_array() {
    t = Analex(fd);
    if (t.cat != CT_INT) {
        erro("Tamanho do array deve ser uma constante inteira.");
    }
    t = Analex(fd);
    if (t.cat != SN || t.codigo != FECHA_COLCH) {
        erro("Esperado ']' para fechar a dimensão do array.");
    }
}

DECL_SINALIZADOR Decl() {
    DECL_SINALIZADOR declFlag = NO_DECL;
    TOKEN idToken;
    TIPO_DADO tipoDado;
    int tipo;
    int contProt = 0;
 
    if (t.cat == PR && (t.codigo == INT || t.codigo == FLOAT || t.codigo == BOOL || t.codigo == CHAR || t.codigo == VOID)) {
        tipo = t.codigo;
        tipoDado = getTokenTipo(tipo);
        t = Analex(fd); 
 
        if (t.cat != ID) {
            erro("Identificador esperado");
        }
        idToken = t; 
 
        if (BuscaTabelaIDMesmoEscopo(idToken.lexema, escopo) != -1) {
            char msg[100];
            sprintf(msg, "Redeclaração do identificador '%s'", idToken.lexema);
            erro(msg);
        }

        if (tLookahead.cat == SN && tLookahead.codigo == ABRE_PAREN) {
            contProt = 1;
            declFlag = DECL_PROT;
            InsereTabelaID(idToken.lexema, CAT_FUNC, escopo, tipoDado, false, false, 0);
            t = Analex(fd);
            escopo = LOCAL;
            t = Analex(fd);
            Tipos_param();
            if (t.cat != SN || t.codigo != FECHA_PAREN) {
                erro("Fecha parênteses de fim de parâmetros de função esperado");
            }
            t = Analex(fd);
            escopo = GLOBAL;
             if (contProt == 1) {
                declFlag = DECL_PROT_UNICO; 
            }
 
        } else { 
            declFlag = DECL_VAR;
            if (tipo == VOID) {
                erro("Variável não pode ser do tipo void");
            }
 
            t = Analex(fd); 
            if (t.cat == SN && t.codigo == ABRE_COLCH) {
                t = Analex(fd); 
                if (t.cat != CT_INT) erro("Tamanho do array deve ser uma constante inteira.");
                int tamArray = t.valor_int;
                InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipoDado, false, true, tamArray);
                t = Analex(fd); 
                if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' para fechar dimensão do array.");
                t = Analex(fd); 
            } else {
                InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipoDado, false, false, 0);
            }

            while (t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd); 
                if (t.cat != ID) erro("Identificador esperado após vírgula");
                idToken = t;
                if (BuscaTabelaIDMesmoEscopo(idToken.lexema, escopo) != -1) {
                    char msg[100];
                    sprintf(msg, "Redeclaração do identificador '%s'", idToken.lexema);
                    erro(msg);
                }
                t = Analex(fd); 
                if (t.cat == SN && t.codigo == ABRE_COLCH) {
                     t = Analex(fd); 
                    if (t.cat != CT_INT) erro("Tamanho do array deve ser uma constante inteira.");
                    int tamArray = t.valor_int;
                    InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipoDado, false, true, tamArray);
                    t = Analex(fd); 
                    if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' para fechar dimensão do array.");
                    t = Analex(fd); 
                } else {
                    InsereTabelaID(idToken.lexema, CAT_VAR, escopo, tipoDado, false, false, 0);
                }
            }
        }
 
    } else {
        erro("Tipo inválido ou ausente na declaração");
    }
    return declFlag;
}

int corpo_func() {
    int var_locais_count = 0;
    offset_local_atual = 0;
    
    escopo = LOCAL;

    
    while (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        TIPO_DADO tipo_atual = getTokenTipo(t.codigo);
        t = Analex(fd);

        processa_lista_declaracao_vars(tipo_atual);
        var_locais_count++;

        while(t.cat == SN && t.codigo == VIRGULA) { 
            t = Analex(fd);
            processa_lista_declaracao_vars(tipo_atual);
            var_locais_count++;
        }
        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' no final da declaração local.");
        }
        t = Analex(fd);
    }

    if (var_locais_count > 0) {
        printf("AMEM %d\n", var_locais_count);
    }

    while(t.cat != SN || t.codigo != FECHA_CHAVES) {
        Cmd();
    }

    escopo = GLOBAL;
    return var_locais_count;
}


void Decl_var() {
    if (t.cat != ID) {
        erro("Esperado identificador na declaração de variável.");
    }
    t = Analex(fd);
    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd);
        if (t.cat != CT_INT) {
            erro("Tamanho do array deve ser uma constante inteira.");
        }
        t = Analex(fd);
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' para fechar dimensão do array.");
        }
        t = Analex(fd);
    }
}


void Tipo() {
    if (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        t = Analex(fd);
    } else {
        erro("Esperado um tipo (int, char, float, bool).");
    }
}

void Tipos_param() {
    if (t.cat == PR && t.codigo == VOID) {
        t = Analex(fd);
        return;
    }
    else {
        Tipo();
        if (t.cat == SN && t.codigo == E_COMERCIAL) {
            t = Analex(fd);
        }
        if (t.cat != ID) erro("Esperado identificador de parâmetro.");
        t = Analex(fd);
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            t = Analex(fd);
            if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
            t = Analex(fd);
        }
        while(t.cat == SN && t.codigo == VIRGULA) {
            t = Analex(fd);
            Tipo();
            if (t.cat == SN && t.codigo == E_COMERCIAL) {
                t = Analex(fd);
            }
            if (t.cat != ID) erro("Esperado identificador de parâmetro.");
            t = Analex(fd);
            if (t.cat == SN && t.codigo == ABRE_COLCH) {
                t = Analex(fd);
                if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
                t = Analex(fd);
            }
        }
    }
}



void cmd_cont(TOKEN id_alvo) {
    if (t.cat == SN && t.codigo == ABRE_PAREN) {
        t = Analex(fd);
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            Expr(); 
            while(t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd);
                Expr();
            }
        }
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' para fechar chamada de função.");
        }
        t = Analex(fd);
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após chamada de função.");
        }
        t = Analex(fd);
    }
    else {
        
        int idx = BuscaTabelaID(id_alvo.lexema);
        if (idx == -1) {
            char msg[100];
            sprintf(msg, "Variável '%s' não declarada.", id_alvo.lexema);
            erro(msg);
        }

        IDENTIFICADOR id = tabelaIdentificadores.identificadores[idx];
        TIPO_DADO tipo_lhs = id.tipo;
        
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            if (!id.array) {
                char msg[100];
                sprintf(msg, "Variável '%s' não é um array.", id.nome);
                erro(msg);
            }
            t = Analex(fd);

            TIPO_DADO tipo_indice = Expr();
            if (tipo_indice != TIPO_INT) {
                erro("O índice de um array deve ser uma expressão do tipo int.");
            }

            if (t.cat != SN || t.codigo != FECHA_COLCH) {
                erro("Esperado ']' em atribuição de array.");
            }
            t = Analex(fd);
        }

        if (t.cat != SN || t.codigo != ATRIB) {
            erro("Esperado '=' em comando de atribuição.");
        }
        t = Analex(fd); // Consome '='
        
        TIPO_DADO tipo_rhs = Expr();
        
        checaCompatibilidadeAtribuicao(tipo_lhs, tipo_rhs, id.nome);
        
        id = tabelaIdentificadores.identificadores[idx];
        printf("STORE %s\n", id.nome);

        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após comando de atribuição.");
        }
        t = Analex(fd); // Consome ';'
    }
}


void Atrib() {
    TOKEN id_alvo;
    if (t.cat != ID) {
        erro("Esperado identificador no comando de atribuição.");
    }
    id_alvo = t;
    t = Analex(fd);
    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd);
        Expr();
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' em atribuição de array.");
        }
        t = Analex(fd);
    }
    if (t.cat != SN || t.codigo != ATRIB) {
        erro("Esperado '=' em comando de atribuição.");
    }
    t = Analex(fd);
    Expr();
    printf("STORE %s\n", id_alvo.lexema);
}


void Cmd() {
    if (t.cat == PR && t.codigo == IF) {
        int rotuloElse, rotuloFim;
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'if'.");
        t = Analex(fd); 
        
        TIPO_DADO tipo_expr_if = Expr(); // <-- MODIFICADO: Captura o tipo da expressão
        checaCondicao("if", tipo_expr_if); // <-- NOVO: Checa se a expressão é válida para uma condição 
        
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'if'.");
        t = Analex(fd); 
 
        rotuloElse = novoRotulo();
        printf("GOFALSE L%d\n", rotuloElse);
        Cmd(); 

        if (t.cat == PR && t.codigo == ELSE) {
            rotuloFim = novoRotulo();
            printf("GOTO L%d\n", rotuloFim);
            printf("LABEL L%d\n", rotuloElse);
            t = Analex(fd); 
            Cmd(); 
            printf("LABEL L%d\n", rotuloFim);
        } else {
            printf("LABEL L%d\n", rotuloElse);
        }
    }
    else if (t.cat == PR && t.codigo == WHILE) {
        int rotuloInicio, rotuloFim;
        rotuloInicio = novoRotulo();
        rotuloFim = novoRotulo();
        printf("LABEL L%d\n", rotuloInicio);
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'while'.");
        t = Analex(fd); 
        
        TIPO_DADO tipo_expr_while = Expr();
        checaCondicao("while", tipo_expr_while);
        
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'while'.");
        t = Analex(fd);
        printf("GOFALSE L%d\n", rotuloFim);
        Cmd(); 

        printf("GOTO L%d\n", rotuloInicio);
        printf("LABEL L%d\n", rotuloFim);
    }
    else if (t.cat == PR && t.codigo == FOR) {
        t = Analex(fd); if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'for'.");
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            Atrib();
        } else {
            t = Analex(fd); 
        }
        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            TIPO_DADO tipo_expr_for = Expr(); 
            checaCondicao("for", tipo_expr_for);
        }
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 2a parte do 'for'.");
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != FECHA_PAREN) Atrib();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' para fechar o 'for'.");
        t = Analex(fd); 
        Cmd();
    }
    else if (t.cat == PR && t.codigo == RETURN) {
        t = Analex(fd);
        
        TIPO_DADO tipo_retorno = TIPO_VAZIO;
        bool tem_expressao = (t.cat != SN || t.codigo != PONTO_VIRGULA);
        
        if (tem_expressao) {
            tipo_retorno = Expr(); 
        }
        
        checaRetornoFuncao(funcao_atual, tipo_retorno, tem_expressao);
        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' após 'return'.");
        t = Analex(fd);
    }
    else if (t.cat == SN && t.codigo == ABRE_CHAVES) {
        t = Analex(fd);
        while(t.cat != SN || t.codigo != FECHA_CHAVES) { 
            Cmd(); 
        }
        t = Analex(fd);
    }
    else if (t.cat == ID) {
        TOKEN id_token = t;
        t = Analex(fd);
        cmd_cont(id_token);
    }
    else {
        erro("Comando inválido.");
    }
}


TIPO_DADO Termo() {
    TIPO_DADO tipo_esq = Fator(); 
    
    while (t.cat == SN && (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO || t.codigo == AND )) {
        TOKEN op = t;
        t = Analex(fd);
        TIPO_DADO tipo_dir = Fator(); 
        
        if (op.codigo == MULTIPLICACAO || op.codigo == DIVISAO) {
            tipo_esq = getTipoResultanteAritmetico(tipo_esq, tipo_dir);
            if (op.codigo == MULTIPLICACAO) printf("MUL\n");
            else if (op.codigo == DIVISAO) printf("DIV\n");

        } else if (op.codigo == AND) {
            tipo_esq = getTipoResultanteLogico(tipo_esq, tipo_dir);

            printf("AND\n");
        }
    }
    return tipo_esq; 
}

TIPO_DADO Expr_simp() {
    if (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO)) {
        t = Analex(fd);
    }
    
    TIPO_DADO tipo_esq = Termo();
    
    while (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == OR)) {
        TOKEN op = t;
        t = Analex(fd);
        TIPO_DADO tipo_dir = Termo();

        if (op.codigo == ADICAO || op.codigo == SUBTRACAO) {
            tipo_esq = getTipoResultanteAritmetico(tipo_esq, tipo_dir);
            if (op.codigo == ADICAO) printf("ADD\n"); 
            else if (op.codigo == SUBTRACAO) printf("SUB\n"); 

        } else if (op.codigo == OR) {
            tipo_esq = getTipoResultanteLogico(tipo_esq, tipo_dir);
            printf("OR\n");
        }
    }
    return tipo_esq; 
}

    
TIPO_DADO Expr() {
    TIPO_DADO tipo_esq = Expr_simp(); 

    if (isOp_rel(t)) {
        TOKEN op = t;
        Op_rel(); // Consome o operador relacional
        TIPO_DADO tipo_dir = Expr_simp(); 

        // A checagem semântica garante que os tipos são comparáveis
        TIPO_DADO tipo_resultado = getTipoResultanteRelacional(tipo_esq, tipo_dir);
        
        // --- INÍCIO DO CÓDIGO COMPLETADO ---

        // Passo 1: Deixar o resultado de (esquerda - direita) na pilha.
        // A pilha agora conterá o resultado de 'tipo_esq - tipo_dir'.
        printf("SUB\n"); 
        
        // Passo 2: Gerar a instrução de comparação específica.
        // Assumimos que a Máquina de Pilha tem opcodes que
        // consomem o resultado da subtração, o comparam com zero,
        // e empilham 1 (true) ou 0 (false).
        switch (op.codigo) {
            case IGUALDADE:
                // Se (a-b) == 0, então a == b.
                printf("EQ\n");
                break;
            case DIFERENTE:
                // Se (a-b) != 0, então a != b.
                printf("NE\n");
                break;
            case MENOR_QUE:
                // Se (a-b) < 0, então a < b.
                printf("LT\n");
                break;
            case MENOR_IGUAL:
                // Se (a-b) <= 0, então a <= b.
                printf("LE\n");
                break;
            case MAIOR_QUE:
                // Se (a-b) > 0, então a > b.
                printf("GT\n");
                break;
            case MAIOR_IGUAL:
                // Se (a-b) >= 0, então a >= b.
                printf("GE\n");
                break;
        }
        // --- FIM DO CÓDIGO COMPLETADO ---
        
        return tipo_resultado; // O tipo da expressão é sempre booleano
    }
    return tipo_esq; 
}


void fator_cont(IDENTIFICADOR func_id) {
    t = Analex(fd); // Consome o '('

    // Vamos assumir que os parâmetros da função estão na tabela de símbolos
    // logo após a própria função. Precisamos encontrar o primeiro.
    int indice_primeiro_param = func_id.endereco + 1;
    int arg_count = 0;

    if (t.cat != SN || t.codigo != FECHA_PAREN) {
        // Processa o primeiro argumento
        arg_count++;
        TIPO_DADO tipo_arg = Expr();
        
        // Validação do primeiro argumento
        if (tabelaIdentificadores.identificadores[indice_primeiro_param].categoria != CAT_PARAM) {
            erro_semantico("Função '%s' não espera argumentos.", func_id.nome);
        }
        TIPO_DADO tipo_param_esperado = tabelaIdentificadores.identificadores[indice_primeiro_param].tipo;
        checaCompatibilidadeAtribuicao(tipo_param_esperado, tipo_arg, "argumento de função");

        // Loop para processar os outros argumentos
        while(t.cat == SN && t.codigo == VIRGULA) {
            t = Analex(fd);
            arg_count++;
            tipo_arg = Expr();

            // Validação dos argumentos subsequentes
            int indice_param_atual = indice_primeiro_param + arg_count - 1;
             if (tabelaIdentificadores.identificadores[indice_param_atual].categoria != CAT_PARAM) {
                erro_semantico("Excesso de argumentos para a função '%s'.", func_id.nome);
            }
            tipo_param_esperado = tabelaIdentificadores.identificadores[indice_param_atual].tipo;
            checaCompatibilidadeAtribuicao(tipo_param_esperado, tipo_arg, "argumento de função");
        }
    }

    // TODO: Adicionar checagem final para ver se o número de argumentos bate com o número de parâmetros.

    if (t.cat != SN || t.codigo != FECHA_PAREN) {
        erro("Esperado ')' para fechar chamada de função.");
    }
    t = Analex(fd); // Consome o ')'
}


void fator_cont_array(IDENTIFICADOR id) {
    if (t.cat != SN || t.codigo != ABRE_COLCH) {
        return;
    }
    
    if (!id.array) {
        erro_semantico("Variável '%s' não é um array e não pode ser indexada.", id.nome);
    }
    
    t = Analex(fd); // Consome o '['
    
    TIPO_DADO tipo_indice = Expr();
    
    if (tipo_indice != TIPO_INT) {
        erro_semantico("O índice de um array deve ser uma expressão do tipo int.");
    }
    
    printf("ADD\n");
    
    printf("LDSTK %d\n", id.escopo == GLOBAL ? 0 : 1);
    
    if (t.cat != SN || t.codigo != FECHA_COLCH) {
        erro("Esperado ']' para fechar acesso ao array.");
    }
    t = Analex(fd); // Consome o ']'
}


TIPO_DADO Fator() {
    TIPO_DADO tipoRetorno = TIPO_VAZIO;

    if (t.cat == CT_INT) {
        printf("PUSH %d\n", t.valor_int);
        t = Analex(fd); 
        return TIPO_INT;
    } 
    else if (t.cat == CT_REAL) {
        printf("PUSH %f\n", t.valor_real);
        t = Analex(fd);
        return TIPO_FLOAT; 
    }
    else if (t.cat == CT_CHAR) {
        printf("PUSH %d\n", (int)t.caractere);
        t = Analex(fd);
        return TIPO_CHAR; 
    }
    else if (t.cat == SN && t.codigo == ABRE_PAREN) {
        t = Analex(fd); 
        tipoRetorno = Expr(); 
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' após expressão em fator.");
        }
        t = Analex(fd);
        return tipoRetorno;
    }
    else if (t.cat == SN && t.codigo == NOT) {
        t = Analex(fd);
        Fator();
        return TIPO_BOOL;
    }
    else if (t.cat == ID) {
        TOKEN idToken = t;
        int idx = BuscaTabelaID(idToken.lexema);
        if (idx == -1) {
            erro("ERRO: Identificador não encontrado.");
        }
        
        IDENTIFICADOR id = tabelaIdentificadores.identificadores[idx];
        
        t = Analex(fd); // Consome o ID
        
        if (t.cat == SN && t.codigo == ABRE_PAREN) {
            if (id.categoria != CAT_FUNC) {
                 erro_semantico("Identificador '%s' não é uma função.", id.nome);
            }
            fator_cont(id);
            return id.tipo;
        } 
        // Se não for uma função, é uma variável.
        else {
             if (id.categoria != CAT_VAR) {
                 erro_semantico("Uso inválido do identificador de função '%s' como variável.", id.nome);
            }
            
            if (id.array) {
                printf("PUSH %d\n", id.endereco);
            } else {
                printf("LOAD %d, %d\n", id.escopo == GLOBAL ? 0 : 1, id.endereco);
            }

            fator_cont_array(id);

            return id.tipo;
        }
    }
    else {
        erro("Fator inválido: esperado constante, id, '!' ou '('");
    }
}


void Op_rel() {
    if (isOp_rel(t)) {
        t = Analex(fd);
    } else {
        erro("Esperado um operador relacional (==, !=, <, etc.).");
    }
}

bool isOp_rel(TOKEN token) {
    return (t.cat == SN &&
        (t.codigo == IGUALDADE   || t.codigo == DIFERENTE ||
         t.codigo == MENOR_QUE    || t.codigo == MAIOR_QUE   ||
         t.codigo == MENOR_IGUAL  || t.codigo == MAIOR_IGUAL));
}