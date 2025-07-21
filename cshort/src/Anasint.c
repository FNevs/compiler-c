#include "Anasint.h" 

ESCOPO escopo;

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
            if (declFlag != DECL_PROT_UNICO) {
                erro("Definição de função inválida ou múltipla na mesma linha");
            }
            t = Analex(fd);
            corpo_func();
            if (!(t.cat == SN && t.codigo == FECHA_CHAVES)) {
                erro("Esperado '}' para fechar o corpo da função");
            }
            t = Analex(fd);
        }
        else {
            erro("Esperado ';' ou '{' após a declaração");
        }
    }
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

void corpo_func() {
    while (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        Tipo();
        Decl_var();
        while(t.cat == SN && t.codigo == VIRGULA) { 
            t = Analex(fd); 
            Decl_var();
        }
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' no final da declaração local.");
        }
        t = Analex(fd);
    }
    while(t.cat != SN || t.codigo != FECHA_CHAVES) {
        Cmd();
    }
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
        printf("STOR %s\n", id_alvo.lexema);
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após comando de atribuição.");
        }
        t = Analex(fd);
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
    printf("STOR %s\n", id_alvo.lexema);
}


void Cmd() {
    if (t.cat == PR && t.codigo == IF) {
        int rotuloElse, rotuloFim;
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'if'.");
        t = Analex(fd); 
        Expr(); 
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
        Expr(); 
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
            Atrib(); // Atrib agora gera o STOR
        } else {
            t = Analex(fd); // Consome o primeiro ';'
        }
        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) Expr();
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 2a parte do 'for'.");
        t = Analex(fd); 
        if (t.cat != SN || t.codigo != FECHA_PAREN) Atrib();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' para fechar o 'for'.");
        t = Analex(fd); Cmd();
    }
    else if (t.cat == PR && t.codigo == RETURN) {
        t = Analex(fd);
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) { Expr(); }
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' após 'return'.");
        t = Analex(fd);
    }
    else if (t.cat == SN && t.codigo == ABRE_CHAVES) {
        t = Analex(fd);
        while(t.cat != SN || t.codigo != FECHA_CHAVES) { Cmd(); }
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


void Expr() {
    Expr_simp(); 
    if (isOp_rel(t)) {
        TOKEN op = t;
        Op_rel(); 
        Expr_simp(); 
        printf("SUB\n"); 
    }
}


void Expr_simp() {
    if (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO)) {
        t = Analex(fd);
    }
    Termo(); 
    while (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == OR)) {
        TOKEN op = t;
        t = Analex(fd);
        Termo(); 
        if (op.codigo == ADICAO) {
            printf("ADD\n"); 
        } else if (op.codigo == SUBTRACAO) {
            printf("SUB\n"); 
        }
    }
}


void Termo() {
    Fator(); 
    while (t.cat == SN && (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO || t.codigo == AND )) {
        TOKEN op = t;
        t = Analex(fd);
        Fator(); 
        if (op.codigo == MULTIPLICACAO) {
            printf("MUL\n");
        } else if (op.codigo == DIVISAO) {
            printf("DIV\n");
        }
    }
}

void fator_cont() {
    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd);
        Expr();
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' para fechar acesso ao array.");
        }
        t = Analex(fd);
    }
    else if (t.cat == SN && t.codigo == ABRE_PAREN) {
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
    }
    else {
        return;
    }
}


void Fator() {
    if (t.cat == CT_INT) {
        printf("PUSH %d\n", t.valor_int);
        t = Analex(fd); 
    } 
    else if (t.cat == CT_REAL) {
        printf("PUSH %f\n", t.valor_real);
        t = Analex(fd);
    }
    else if (t.cat == CT_CHAR) {
        printf("PUSH %d\n", (int)t.caractere);
        t = Analex(fd);
    }
    else if (t.cat == SN && t.codigo == ABRE_PAREN) {
        t = Analex(fd); 
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' após expressão em fator.");
        }
        t = Analex(fd);
    }
    else if (t.cat == SN && t.codigo == NOT) {
        t = Analex(fd);
        Fator();
    }
    else if (t.cat == ID) {
        TOKEN idToken = t;
        int idx = BuscaTabelaID(idToken.lexema);
        if (idx == -1) {
            char msg[100];
            sprintf(msg, "Identificador '%s' não declarado", idToken.lexema);
            erro(msg);
        }
        printf("LOAD %s\n", idToken.lexema);
        t = Analex(fd);
        fator_cont();
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