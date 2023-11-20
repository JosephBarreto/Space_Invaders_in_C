#include <stdlib.h>
#include "shot.h"

lista_tiros* cria_lista_compart() {
    lista_tiros* lista;

    if (!(lista = (lista_tiros*) malloc(sizeof(lista_tiros))))
        return NULL;

    lista->tiros = NULL;

    return lista;
}

tiro* cria_tiro(unsigned short x, unsigned short y, unsigned short traject, unsigned short dano, unsigned short tipo, tiro* prox){
    tiro* novo_tiro;

    if(!(novo_tiro = (tiro*) malloc(sizeof(tiro))))
        return NULL;

    novo_tiro->x = x;
    novo_tiro->y = y;
    novo_tiro->trajeto = traject;
    novo_tiro->dano_tiro = dano;
    novo_tiro->tipo_tiro = tipo;
    novo_tiro->prox = (struct tiro* ) prox;
    return novo_tiro;
}

void destroi_tiros(tiro* inicio){
    tiro* aux = inicio; 
    
    while(aux){
        inicio = (tiro* ) aux->prox;
        free(aux);
        aux = inicio;
    }
}

void destroi_um_tiro(tiro* shot, tiro* ant){
    if(!shot) return;
    if(ant) ant->prox = shot->prox;
    free(shot);
}