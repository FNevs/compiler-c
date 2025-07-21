Iniciando análise sintática do arquivo: teste.cshort

INIP
LOAD a
LOAD b
SUB
GOFALSE L0
PUSH 20
STOR resultado
GOTO L1
LABEL L0
PUSH 0
STOR resultado
LABEL L1
LABEL L2
LOAD a
PUSH 0
SUB
GOFALSE L3
LOAD a
PUSH 1
SUB
STOR a
GOTO L2
LABEL L3
HALT

Análise sintática concluída com sucesso! O arquivo 'teste.cshort' é sintaticamente válido.
