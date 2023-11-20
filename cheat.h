#ifndef __CHEAT__
#define __CHEAT__

typedef struct{ 
    unsigned short comandos[10];
    unsigned short size_cod;
    unsigned short efeito;
    unsigned short execucao;
    struct codigo* prox;
} codigo;

codigo* cria_codigos();
int le_codigo(codigo* trapaca, int cheats, int key);
int executa_codigo(codigo* ativado);

#endif