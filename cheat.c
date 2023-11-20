#include <stdlib.h>
#include <stdio.h>
#include "cheat.h"

codigo* cria_codigos(){
    codigo* red, *blue, *black;
    
    if(!(red = (codigo*) malloc(sizeof(codigo))))
        return NULL;
    
    if(!(blue = (codigo*) malloc(sizeof(codigo))))
        return NULL;

    if(!(black = (codigo*) malloc(sizeof(codigo))))
        return NULL;


    red->comandos[0] = 18;
    red->comandos[1] = 5;
    red->comandos[2] = 4;
    red->size_cod = 3;
    red->efeito = 1;
    red->execucao = 0;
    red->prox = (struct codigo*) blue;

    blue->comandos[0] = 2;
    blue->comandos[1] = 12;
    blue->comandos[2] = 21;
    blue->comandos[3] = 5;
    blue->size_cod = 4;
    blue->efeito = 2;
    blue->execucao = 0;
    blue->prox = (struct codigo*) black;

    black->comandos[0] = 2;
    black->comandos[1] = 12;
    black->comandos[2] = 1;
    black->comandos[3] = 3;
    black->comandos[4] = 11;
    black->size_cod = 5;
    black->efeito = 3;
    black->execucao = 0;
    black->prox = NULL;

    return red;
}

int executa_codigo(codigo* ativado){

    if(!ativado)
    return 0;

    switch(ativado->efeito)
    {
        case 1: return 11;
        break;
        case 2: return 12;
        break;        
        case 3: return 13;
        break;
    }

    return 0;
}


int le_codigo(codigo* trapaca, int cheats, int key){
    codigo* aux;
    int detecta = 0;

    aux = trapaca;
    while(aux){
        
        if (key == aux->comandos[cheats]){
            if(!cheats) aux->execucao++;
            detecta = 1;
            aux = (codigo*) aux->prox;
            continue;
        }
        
        if(aux->execucao) aux->execucao = 0;
        
        aux = (codigo*) aux->prox;
    }

    if(detecta) cheats++;
    else cheats = 0;
    
    aux = trapaca;
    while(aux){
        if ((cheats == aux->size_cod) && (key == aux->comandos[cheats-1] && (aux->execucao >= 1)))
            cheats = executa_codigo(aux);
        aux = (codigo*) aux->prox;    
    }
    return cheats;
}

