/*
  Este arquivo irá conter a implementação do Analisador Sintático
  Descendente Recursivo (ASDR). Cada função corresponde a um
  não-terminal da gramática abaixo.

======================================================================
  Regras de Produção da Gramática Cshort (da Especificação v1.0)
======================================================================

<prog>      ::= { <decl> ';' | <func> }

<decl>      ::= <tipo> <decl_var> { ',' <decl_var> }
              | <tipo> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }
              | <void> <id> '(' <tipos_param> ')' { ',' <id> '(' <tipos_param> ')' }

<decl_var>  ::= <id> [ '[' <intcon> ']' ]

<tipo>      ::= 'char' | 'int' | 'float' | 'bool'

<tipos_param> ::= <void>
                | <tipo> [ '&' ] <id> [ '[' ']' ] { ',' <tipo> [ '&' ] <id> [ '[' ']' ] }

<func>      ::= <tipo> <id> '(' <tipos_param> ')' '{' { <tipo> <decl_var> {',' <decl_var>} ';'} { <cmd> } '}'
              | <void> <id> '(' <tipos_param> ')' '{' { <tipo> <decl_var> {',' <decl_var>} ';'} { <cmd> } '}'

<cmd>       ::= 'if' '(' <expr> ')' <cmd> [ 'else' <cmd> ]
              | 'while' '(' <expr> ')' <cmd>
              | 'for' '(' [ <atrib> ] ';' [ <expr> ] ';' [ <atrib> ] ')' <cmd>
              | 'return' [ <expr> ] ';'
              | <atrib> ';'
              | <id> '(' [ <expr> { ',' <expr> } ] ')' ';'
              | '{' { <cmd> } '}'

<atrib>     ::= <id> [ '[' <expr> ']' ] '=' <expr>

<expr>      ::= <expr_simp> [ <op_rel> <expr_simp> ]

<expr_simp> ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

<termo>     ::= <fator> { ( '*' | '/' | '&&' ) <fator> }

<fator>     ::= <id> [ '[' <expr> ']' ]
              | <intcon> | <realcon> | <charcon>
              | <id> '(' [ <expr> { ',' <expr> } ] ')'
              | '(' <expr> ')'
              | '!' <fator>

<op_rel>    ::= '==' | '!=' | '<=' | '<' | '>=' | '>'

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
    // A função é chamada após um ID, quando se sabe que um '[' virá.
    
    // Consome '['
    t = Analex(fd); 

    // Verifica se há uma constante inteira para o tamanho
    if (t.cat != CT_INT) {
        erro("Tamanho do array deve ser uma constante inteira.");
    }
    t = Analex(fd); // consome a constante inteira

    // Verifica o fechamento de colchetes
    if (t.cat != SN || t.codigo != FECHA_COLCH) {
        erro("Esperado ']' para fechar a dimensão do array.");
    }
    t = Analex(fd); // consome ']'
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

        if (tLookahead.cat==SN && tLookahead.codigo==ABRE_COLCH) {
            declFlag = DECL_VAR;

            if (tipo == VOID) {
                erro("Tipo void inválido");
            }

            trata_array();
        }

        if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) {
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
            t = Analex(fd); // consome ´(´
            escopo = LOCAL;
            t = Analex(fd); 
            Tipos_param();

            if (t.cat != SN || t.codigo != FECHA_PAREN) {
                erro("Fecha parênteses de fim de parâmetros de função esperado");
            }

            escopo = GLOBAL;

            if (tLookahead.cat == SN && tLookahead.codigo == VIRGULA) {
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

                    t = Analex(fd); // consome ´(´
                    escopo = LOCAL;
                    t = Analex(fd); 
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
    // Parte 1: Declarações de variáveis locais
    while (t.cat == PR && (t.codigo == INT || t.codigo == CHAR || t.codigo == FLOAT || t.codigo == BOOL)) {
        Tipo(); // Consome o tipo
        
        Decl_var(); // Chama a função para o primeiro decl_var
        
        // Trata os outros decl_var (se houver)
        while(t.cat == SN && t.codigo == VIRGULA) {
            t = Analex(fd); // consome ','
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
        t = Analex(fd); // consome '['
        if (t.cat != CT_INT) {
            erro("Tamanho do array deve ser uma constante inteira.");
        }
        t = Analex(fd); // consome intcon
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' para fechar dimensão do array.");
        }
        t = Analex(fd); // consome ']'
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
        // Parâmetro por referência opcional
        if (t.cat == SN && t.codigo == E_COMERCIAL) {
            t = Analex(fd); // Consome '&'
        }
        if (t.cat != ID) erro("Esperado identificador de parâmetro.");
        t = Analex(fd); // Consome o ID
        // Array opcional
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            t = Analex(fd); // consome '['
            if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
            t = Analex(fd); // consome ']'
        }

        // Demais parâmetros (opcionais, com vírgula)
        while(t.cat == SN && t.codigo == VIRGULA) {
            t = Analex(fd); // consome ','
            Tipo();
            if (t.cat == SN && t.codigo == E_COMERCIAL) {
                t = Analex(fd); // Consome '&'
            }
            if (t.cat != ID) erro("Esperado identificador de parâmetro.");
            t = Analex(fd); // Consome o ID
            if (t.cat == SN && t.codigo == ABRE_COLCH) {
                t = Analex(fd); // consome '['
                if (t.cat != SN || t.codigo != FECHA_COLCH) erro("Esperado ']' em parâmetro de array.");
                t = Analex(fd); // consome ']'
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

        // Trata a lista de parâmetros (opcional)
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            Expr();
            while(t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd); // consome a ','
                Expr();
            }
        }

        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' para fechar chamada de função.");
        }
        t = Analex(fd); // consome ')'

        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após chamada de função.");
        }
        t = Analex(fd); // consome ';'
    }
    // Alternativa 2: Comando de atribuição
    else {
        // Se não for '(', tem que ser uma atribuição.
        // A regra é [ '[' <expr> ']' ] '=' <expr> ';'
        // A parte do array é opcional
        if (t.cat == SN && t.codigo == ABRE_COLCH) {
            t = Analex(fd); // consome '['
            Expr();
            if (t.cat != SN || t.codigo != FECHA_COLCH) {
                erro("Esperado ']' em atribuição de array.");
            }
            t = Analex(fd); // consome ']'
        }

        if (t.cat != SN || t.codigo != ATRIB) {
            erro("Esperado '=' em comando de atribuição.");
        }
        t = Analex(fd); // consome '='
        Expr();

        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            erro("Esperado ';' após comando de atribuição.");
        }
        t = Analex(fd); // consome ';'
    }
}


// Implementa um comando de atribuição (usado no 'for')
void Atrib() {
    // Regra: <atrib> ::= <id> [ '[' <expr> ']' ] '=' <expr>
    // Esta função assume que o ID já foi verificado ou consumido
    // Para o 'for', vamos verificar aqui.
    if (t.cat != ID) {
        erro("Esperado identificador no comando de atribuição.");
    }
    t = Analex(fd); // consome ID

    if (t.cat == SN && t.codigo == ABRE_COLCH) {
        t = Analex(fd); // consome '['
        Expr();
        if (t.cat != SN || t.codigo != FECHA_COLCH) {
            erro("Esperado ']' em atribuição de array.");
        }
        t = Analex(fd); // consome ']'
    }

    if (t.cat != SN || t.codigo != ATRIB) {
        erro("Esperado '=' em comando de atribuição.");
    }
    t = Analex(fd); // consome '='
    Expr();
}


void Cmd() {
    // Distribuidor para os vários tipos de comando

    // Comando 'if'
    if (t.cat == PR && t.codigo == IF) {
        t = Analex(fd); // consome 'if'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'if'.");
        t = Analex(fd); // consome '('
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'if'.");
        t = Analex(fd); // consome ')'
        Cmd(); // Processa o comando do bloco 'if'
        
        // Parte opcional 'else'
        if (t.cat == PR && t.codigo == ELSE) {
            t = Analex(fd); // consome 'else'
            Cmd(); // Processa o comando do bloco 'else'
        }
    }
    // Comando 'while'
    else if (t.cat == PR && t.codigo == WHILE) {
        t = Analex(fd); // consome 'while'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'while'.");
        t = Analex(fd); // consome '('
        Expr();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' após expressão do 'while'.");
        t = Analex(fd); // consome ')'
        Cmd();
    }
    // Comando 'for'
    else if (t.cat == PR && t.codigo == FOR) {
        t = Analex(fd); // consome 'for'
        if (t.cat != SN || t.codigo != ABRE_PAREN) erro("Esperado '(' após 'for'.");
        t = Analex(fd); // consome '('
        // 1a parte: [atrib]
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) Atrib();
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 1a parte do 'for'.");
        t = Analex(fd); // consome ';'
        // 2a parte: [expr]
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) Expr();
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' na 2a parte do 'for'.");
        t = Analex(fd); // consome ';'
        // 3a parte: [atrib]
        if (t.cat != SN || t.codigo != FECHA_PAREN) Atrib();
        if (t.cat != SN || t.codigo != FECHA_PAREN) erro("Esperado ')' para fechar o 'for'.");
        t = Analex(fd); // consome ')'
        Cmd();
    }
    // Comando 'return'
    else if (t.cat == PR && t.codigo == RETURN) {
        t = Analex(fd); // consome 'return'
        // Expressão opcional
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) {
            Expr();
        }
        if (t.cat != SN || t.codigo != PONTO_VIRGULA) erro("Esperado ';' após 'return'.");
        t = Analex(fd); // consome ';'
    }
    // Bloco de comandos
    else if (t.cat == SN && t.codigo == ABRE_CHAVES) {
        t = Analex(fd); // consome '{'
        // Processa múltiplos comandos dentro do bloco
        while(t.cat != SN || t.codigo != FECHA_CHAVES) {
            Cmd();
        }
        t = Analex(fd); // consome '}'
    }
    // Comando iniciado por ID (atribuição ou chamada de função)
    else if (t.cat == ID) {
        t = Analex(fd); // consome o ID
        cmd_cont(); // Deixa a função auxiliar decidir
    }
    // Se não for nenhum dos anteriores, é um erro.
    else {
        erro("Comando inválido.");
    }
}

// A NOVA função Expr (ponto de entrada para expressões)
// Trata o operador de menor precedência: '||'
void Expr() {
    // Regra: <expr> ::= <expr_and> { '||' <expr_and> }
    Expr_and(); // Chama o próximo nível de precedência
    while (t.cat == SN && t.codigo == OR) {
        t = Analex(fd);
        Expr_and();
    }
}

// A NOVA função Expr_and
// Trata o operador '&&'
void Expr_and() {
    // Regra: <expr_and> ::= <expr_rel> { '&&' <expr_rel> }
    Expr_rel(); // Chama o próximo nível de precedência
    while (t.cat == SN && t.codigo == AND) {
        t = Analex(fd);
        Expr_rel();
    }
}

void Expr_rel() {
    // Regra: <expr> ::= <expr_simp> [ <op_rel> <expr_simp> ]
    Expr_simp();

    // A verificação se a parte opcional existe continua a mesma
    if (t.cat == SN &&
        (t.codigo == IGUALDADE   || t.codigo == DIFERENTE ||
         t.codigo == MENOR_QUE    || t.codigo == MAIOR_QUE   ||
         t.codigo == MENOR_IGUAL  || t.codigo == MAIOR_IGUAL)) {
        
        Op_rel(); // Chama a função dedicada para consumir o operador
        Expr_simp();
    }
}

void Expr_simp() {
    // Regra de produção: <expr_simp> ::= [ '+'|'-' ] <termo> { ( '+' | '-' | '||' ) <termo> }

    // 1. Trata a parte opcional do sinal unário no início: [ '+'|'-' ]
    // A regra de precedência define que o '-' unário tem alta precedência,
    // mas a gramática o coloca aqui. Vamos seguir a gramática.
    if (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO)) {
        // Apenas consome o sinal. A ação semântica (ex: negação) viria depois.
        t = Analex(fd);
    }

    // 2. Processa a primeira (e obrigatória) parte <termo>
    Termo();

    // 3. Trata a parte repetitiva { ( '+' | '-' | '||' ) <termo> } com um laço.
    // Esta parte lida com operadores de menor precedência.
    while (t.cat == SN &&
           (t.codigo == ADICAO || t.codigo == SUBTRACAO)) {
        
        // Consome o operador (+, - ou ||)
        t = Analex(fd);

        // Processa o próximo <termo> na sequência
        Termo();
    }
    
    // O laço termina quando não há mais operadores de adição/subtração/or,
    // e a função retorna.
}


void Termo() {
    // Regra de produção: <termo> ::= <fator> { ( '*' | '/' | '&&' ) <fator> }

    // 1. Processa a primeira (e obrigatória) parte <fator>
    Fator();

    // 2. Trata a parte repetitiva { ( '*' | '/' | '&&' ) <fator> } com um laço.
    // Esta parte lida com operadores de maior precedência que a adição/subtração.
    while (t.cat == SN &&
           (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO )) {

        // Consome o operador (*, / ou &&)
        t = Analex(fd);

        // Processa o próximo <fator> na sequência
        Fator();
    }
    // O laço termina quando não há mais operadores de multiplicação/divisão/and,
    // e a função retorna.
}


void fator_cont() {
    // Regra: <fator_cont> ::= '[' <expr> ']' | '(' [ <expr> {',' <expr>} ] ')' | ε
    // Esta função é chamada DEPOIS de um ID já ter sido consumido.
    // Olhamos o token ATUAL ('t') para decidir o que fazer.

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
        // A lista é opcional. Se o próximo token não for ')', então temos parâmetros.
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            Expr(); // Processa o primeiro parâmetro (expressão)
            // Enquanto houver vírgulas, processa os próximos parâmetros
            while(t.cat == SN && t.codigo == VIRGULA) {
                t = Analex(fd); // consome a ','
                Expr();
            }
        }
        
        if (t.cat != SN || t.codigo != FECHA_PAREN) {
            erro("Esperado ')' para fechar chamada de função.");
        }
        // Consome o ')'
        t = Analex(fd);
    }
    // Alternativa 3: ε (epsilon)
    else {
        // Se não for nem '[' nem '(', o fator era apenas uma variável simples.
        // O ID já foi consumido pela função Fator().
        // Não fazemos nada, apenas retornamos. O token 't' atual pertence
        // a outra regra e não deve ser consumido aqui.
        return;
    }
}


void Fator() {
    // Regra: <fator> ::= <intcon> | <realcon> | <charcon> | '(' <expr> ')' | '!' <fator> | <id> <fator_cont>
    
    // Verifica qual tipo de fator temos com base no token atual 't'
    if (t.cat == CT_INT || t.cat == CT_REAL || t.cat == CT_CHAR) {
        // Casos: <intcon> | <realcon> | <charcon>
        // Se for uma constante, apenas consumimos o token.
        t = Analex(fd);
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
        Fator(); // Chamada recursiva para o fator seguinte
    }
    else if (t.cat == ID) {
        // Caso: <id> <fator_cont>
        // O tratamento foi fatorado. Primeiro consumimos o ID...
        t = Analex(fd);
        // ...e então chamamos a função auxiliar para tratar o que vem depois.
        fator_cont();
    }
    else {
        // Se não for nenhum dos casos acima, a expressão está mal formada.
        erro("Fator inválido: esperado constante, id, '!' ou '('");
    }
}

void Op_rel() {
    // Regra: <op_rel> ::= '==' | '!=' | '<=' | '<' | '>=' | '>'
    if (t.cat == SN &&
        (t.codigo == IGUALDADE   || t.codigo == DIFERENTE ||
         t.codigo == MENOR_QUE    || t.codigo == MAIOR_QUE   ||
         t.codigo == MENOR_IGUAL  || t.codigo == MAIOR_IGUAL)) {

        t = Analex(fd); // Consome o token do operador
    } else {
        erro("Esperado um operador relacional (==, !=, <, etc.).");
    }
}