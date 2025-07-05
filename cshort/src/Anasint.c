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

ESCOPO escopo;

void Prog() {

    DECL_SINALIZADOR declFlag;

    t = Analex(fd);
    while (t.cat != FIM_ARQ) {
        declFlag = Decl();
        t = Analex(fd);
        
        if (t.cat==SN) {
            if (t.codigo==PONTO_VIRGULA) {
                t = Analex(fd);
            }
            else if (t.codigo==ABRE_CHAVES) {

                if (declFlag!=DECL_PROT_UNICO) {
                    erro("Definição de função inválida");
                }

                t = Analex(fd);
                corpo_func();

                if (!(t.cat==SN && t.codigo==FECHA_CHAVES)) {
                    erro("Fecha chaves esperado");
                }

                t = Analex(fd);

            }
            else {
                erro("Ponto e virgula esperado");
            }
        }
    }
}

void trata_array() {
    t = Analex(fd); // Consome '['

    if (t.cat != CT_INT) {
        erro("Tamanho do array deve ser uma constante inteira.");
    }
    t = Analex(fd); // Consome a CT_INT

    if (t.cat != SN || t.codigo != FECHA_COLCH) {
        erro("Esperado ']' para fechar a dimensão do array.");
    }
    t = Analex(fd); // Consome ']'
}


DECL_SINALIZADOR Decl() {
    DECL_SINALIZADOR declFlag = NO_DECL;
    int tipo;
    int contProt = 0;

    if (t.cat == PR && 
        (t.codigo == INT || t.codigo == FLOAT || t.codigo == BOOL ||
        t.codigo == CHAR || t.codigo == VOID)) {
        tipo = t.codigo;

        t = Analex(fd);

        if (t.cat != ID) {
            erro("Identificador esperado");
        }

        if (tLookahead.cat==SN && tLookahead.codigo==ABRE_COLCH) { // Está criando um array
            declFlag = DECL_VAR;

            if (tipo == VOID) {
                erro("Tipo void inválido");
            }

            trata_array();
        }

        if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) { // Criando outras variaveis do mesmo tipo
            declFlag = DECL_VAR;

            if (tipo == VOID) {
                erro("Tipo void inválido");
            }

            t = Analex(fd);

            while (t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd);
                if (t.cat != ID) {
                    erro("Identificador esperado");
                }

                if (tLookahead.cat==SN && tLookahead.codigo==ABRE_COLCH) {
                    trata_array();
                }
                t = Analex(fd); 
            }
        }
        else if (tLookahead.cat == SN && tLookahead.codigo == ABRE_PAREN) {

            if (declFlag==DECL_VAR) {
                erro("Declaração simultânea de variável e função inválida");
            }

            declFlag = DECL_PROT;
            contProt++;
            t = Analex(fd); // Consome ´(´
            escopo = LOCAL;
            t = Analex(fd);
            Tipos_param();

            if (t.cat != SN || t.codigo != FECHA_PAREN) { // MUDAMOS DO CODIGO ORIGINAL : (t.cat != SN || t.codigo == FECHA_PAREN)
                erro("Fecha parênteses de fim de parâmetros de função esperado");
            }

            if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) { // Declara outras variaveis
                t = Analex(fd);
                while (t.cat == SN && t.codigo == VIRGULA) {
                    contProt++;
                    t = Analex(fd);

                    if (t.cat != ID) {
                        erro("Identificador esperado");
                    }

                    t = Analex(fd);

                    if (t.cat != SN || t.codigo != ABRE_PAREN) {
                        erro("Declaração de parâmetros de função esperado");
                    }

                    t = Analex(fd); // Consome ´(´
                    escopo = LOCAL;
                    Tipos_param();

                    if (t.cat != SN || t.codigo != FECHA_PAREN) {
                        erro("Fecha parênteses de fim de parâmetros de função esperado");
                    }

                    escopo = GLOBAL;
                    t = Analex(fd);
                }
            }

            if (contProt == 1) {
                declFlag = DECL_PROT_UNICO; 
            }
        } else {
            declFlag = DECL_VAR;
        }
    } else {
        erro("Tipo inválido");
    }

    return declFlag;
}

void corpo_func() {
    // Declarações de variáveis locais
    while (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        Tipo(); // Consome o tipo
        
        Decl_var();
        
        while(t.cat == SN && t.codigo == VIRGULA) { // Trata os outros decl_va
            t = Analex(fd); // Consome ','
            Decl_var(); // Chama a função para os próximos
        }
        
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' no final da declaração local.");
        }
        t = Analex(fd); // Consome ';'
    }

    // Parte 2: Comandos
    while(t.cat != SN || t.codigo != FECHA_CHAVES) {
        Cmd();
    }

}


void Decl_var() {
    // Regra: <decl_var> ::= <id> [ '[' <intcon> ']' ]
    if (t.cat != ID) {
        erro("Esperado identificador na declaração de variável.");
    }
    t = Analex(fd); // Consome o ID

    // Parte opcional para array
    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd); // Consome '['
        if (t.cat != CT_INT) {
            erro("Tamanho do array deve ser uma constante inteira.");
        }
        t = Analex(fd); // Consome intcon
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' para fechar dimensão do array.");
        }
        t = Analex(fd); // Consome ']'
    }
}


void Tipo() {
    // Regra: <tipo> ::= 'char' | 'int' | 'float' | 'bool'
    if (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        t = Analex(fd); // Consome o token de tipo
    } else {
        erro("Esperado um tipo (int, char, float, bool).");
    }
}

void Tipos_param() {
    // Regra: <tipos_param> ::= void | <tipo> ...

    // Caso 1: sem parâmetros (void)
    if (t.cat == PR && t.codigo == VOID) {
        t = Analex(fd); // Consome 'void'
        return;
    }
    
    // Caso 2: lista de um ou mais parâmetros
    else {
        // Primeiro parâmetro (obrigatório se não for void)
        Tipo();

        if (t.cat == SN && t.codigo == E_COMERCIAL) {
            t = Analex(fd); // Consome '&'
        }
        if (t.cat != ID) erro("Esperado identificador de parâmetro.");
        t = Analex(fd); // Consome o ID
        // Array opcional
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            t = Analex(fd); // Consome '['
            if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
            t = Analex(fd); // Consome ']'
        }

        while(t.cat == SN && t.codigo == VIRGULA) { // Outros parâmetros
            t = Analex(fd); // Consome ','
            Tipo();
            if (t.cat == SN && t.codigo == E_COMERCIAL) {
                t = Analex(fd); // Consome '&'
            }
            if (t.cat != ID) erro("Esperado identificador de parâmetro.");
            t = Analex(fd); // Consome o ID
            if (t.cat == SN && t.codigo == ABRE_COLCH) {
                t = Analex(fd); // Consome '['
                if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
                t = Analex(fd); // Consome ']'
            }
        }
    }
}

void cmd_cont() {
    // Regra: <cmd_cont> ::= '(' [ <expr> {',' <expr>} ] ')' ';' | [ '[' <expr> ']' ] '=' <expr> ';'
    // Chamada DEPOIS que um ID foi consumido.

    // Alternativa 1: Chamada de função
    if (t.cat == SN && t.codigo == ABRE_PAREN) {
        // Consome o '('
        t = Analex(fd);

        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            Expr();
            while(t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd); // Consome a ','
                Expr();
            }
        }

        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' para fechar chamada de função.");
        }
        t = Analex(fd); // Consome ')'

        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após chamada de função.");
        }
        t = Analex(fd); // Consome ';'
    }
    // Alternativa 2: Comando de atribuição
    else {
        // Se não for '(', tem que ser uma atribuição.
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            t = Analex(fd); // Consome '['
            Expr();
            if (t.cat != SN || t.codigo != FECHA_COLCH) {
                erro("Esperado ']' em atribuição de array.");
            }
            t = Analex(fd); // Consome ']'
        }

        if (t.cat != SN || t.codigo != ATRIB) {
            erro("Esperado '=' em comando de atribuição.");
        }
        t = Analex(fd); // Consome '='
        Expr();

        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após comando de atribuição.");
        }
        t = Analex(fd); // Consome ';'
    }
}


void Atrib() {
    // Regra: <atrib> ::= <id> [ '[' <expr> ']' ] '=' <expr>

    if (t.cat != ID) {
        erro("Esperado identificador no comando de atribuição.");
    }
    t = Analex(fd); // Consome ID

    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd); // Consome '['
        Expr();
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' em atribuição de array.");
        }
        t = Analex(fd); // Consome ']'
    }

    if (t.cat != SN || t.codigo != ATRIB) {
        erro("Esperado '=' em comando de atribuição.");
    }
    t = Analex(fd); // Consome '='
    Expr();
}


void Cmd() {
    // Comando 'if'
    if (t.cat == PR && t.codigo == IF) {
        t = Analex(fd); // Consome 'if'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'if'.");
        t = Analex(fd); // Consome '('
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'if'.");
        t = Analex(fd); // Consome ')'
        Cmd(); // Processa o comando do bloco 'if'
        
        // Parte opcional 'else'
        if (t.cat == PR && t.codigo == ELSE) {
            t = Analex(fd); // Consome 'else'
            Cmd(); // Processa o comando do bloco 'else'
        }
    }
    // Comando 'while'
    else if (t.cat == PR && t.codigo == WHILE) {
        t = Analex(fd); // Consome 'while'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'while'.");
        t = Analex(fd); // Consome '('
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'while'.");
        t = Analex(fd); // Consome ')'
        Cmd();
    }
    // Comando 'for'
    else if (t.cat == PR && t.codigo == FOR) {
        t = Analex(fd); // Consome 'for'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'for'.");
        t = Analex(fd); // Consome '('
        // 1a parte: [atrib]
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) Atrib();
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 1a parte do 'for'.");
        t = Analex(fd); // Consome ';'
        // 2a parte: [expr]
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) Expr();
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 2a parte do 'for'.");
        t = Analex(fd); // Consome ';'
        // 3a parte: [atrib]
        if (t.cat != SN || t.codigo != FECHA_PAREN) Atrib();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' para fechar o 'for'.");
        t = Analex(fd); // Consome ')'
        Cmd();
    }
    // Comando 'return'
    else if (t.cat == PR && t.codigo == RETURN) {
        t = Analex(fd); // Consome 'return'
        // Expressão opcional
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            Expr();
        }
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' após 'return'.");
        t = Analex(fd); // Consome ';'
    }
    // Bloco de comandos
    else if (t.cat == SN && t.codigo == ABRE_CHAVES) {
        t = Analex(fd); // Consome '{'
        // Processa múltiplos comandos dentro do bloco
        while(t.cat != SN || t.codigo != FECHA_CHAVES) {
            Cmd();
        }
        t = Analex(fd); // Consome '}'
    }
    // Comando iniciado por ID (atribuição ou chamada de função)
    else if (t.cat == ID) {
        t = Analex(fd); // Consome o ID
        cmd_cont();
    }
    // Se não for nenhum dos anteriores, é um erro.
    else {
        erro("Comando inválido.");
    }
}


void Expr() {
    // Regra: <expr> ::= <expr_simp> [ <op_rel> <expr_simp> ]

    Expr_simp();

    if (isOp_rel(tLookahead)) {
        Op_rel();
        Expr_simp();
    }
}

void Expr_simp() {
    // Regra de produção: <expr_simp> ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

    if (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO)) {
        t = Analex(fd);
    }

    Termo();

    while (t.cat == SN &&
           (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == OR)) {
        
        // Consome o operador (+, - ou ||)
        t = Analex(fd);

        Termo();
    }
}


void Termo() {
    // Regra de produção: <termo> ::= <fator> { ( '*' | '/' | '&&') <fator> }

    Fator();

    while (t.cat == SN &&
           (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO || t.codigo == AND )) {

        // Consome o operador (*, / ou &&)
        t = Analex(fd);

        Fator();
    }
}


void fator_cont() {
    // Regra: <fator_cont> ::= '[' <expr> ']' | '(' [ <expr> {',' <expr>} ] ')' | ε

    // Alternativa 1: Acesso a array
    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        // Consome o '['
        t = Analex(fd);
        Expr();
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' para fechar acesso ao array.");
        }
        // Consome o ']'
        t = Analex(fd);
    }
    // Alternativa 2: Chamada de função
    else if (t.cat == SN && t.codigo == ABRE_PAREN) {
        // Consome o '('
        t = Analex(fd);
        
        // Trata a lista de parâmetros: [ <expr> { ',' <expr> } ]
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            Expr();
            
            while(t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd); // Consome a ','
                Expr();
            }
        }
        
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' para fechar chamada de função.");
        }
        // Consome o ')'
        t = Analex(fd);
    }
    else {
        // Se não for nem '[' nem '(', o fator era apenas uma variável simples.
        return;
    }
}


void Fator() {
    // Regra: <fator> ::= <intcon> | <realcon> | <charcon> | '(' <expr> ')' | '!' <fator> | <id> <fator_cont>
    
    if (t.cat == CT_INT || t.cat == CT_REAL || t.cat == CT_CHAR) {
        // Casos: <intcon> | <realcon> | <charcon>
        t = Analex(fd); // Consome a constante
    } 
    else if (t.cat == SN && t.codigo == ABRE_PAREN) {
        // Caso: '(' <expr> ')'
        t = Analex(fd); // Consome '('
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' após expressão em fator.");
        }
        t = Analex(fd); // Consome ')'
    }
    else if (t.cat == SN && t.codigo == NOT) {
        // Caso: '!' <fator>
        t = Analex(fd); // Consome '!'
        Fator();
    }
    else if (t.cat == ID) {
        // Caso: <id> <fator_cont>
        t = Analex(fd); // Consome ID

        fator_cont();
    }
    else {
        // Se não for nenhum dos casos acima, a expressão está mal formada.
        erro("Fator inválido: esperado constante, id, '!' ou '('");
    }
}

void Op_rel() {
    // Regra: <op_rel> ::= '==' | '!=' | '<=' | '<' | '>=' | '>'
    if (isOp_rel(t)) {
        t = Analex(fd); // Consome o token do operador
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