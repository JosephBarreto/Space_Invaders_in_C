#include <stdlib.h>
#include <stdio.h>
#include "shot.h"
#include "obstaculo.h"

obstacle* cria_obstaculo(int x, int y, int tipo, obstacle* lista){
    obstacle* novo_ob, *aux;

    if(!(novo_ob = (obstacle*) malloc(sizeof(obstacle))))
        return NULL;

    novo_ob->obs_x = x;
    novo_ob->obs_y = y;
    novo_ob->obs_max_x = x+23;
    novo_ob->obs_max_y = y+22;
    novo_ob->vida = 10;
    novo_ob->obs_type = tipo;
    novo_ob->prox = NULL;
    if(lista){

        aux = lista;
        while(aux->prox) aux = (obstacle*) aux->prox;
        aux->prox = (struct obstacle*) novo_ob;
    }


    return novo_ob;

}

obstacle* dano_obstaculo(obstacle* obstaculo, int dano) {
    int vida_ant;

    vida_ant = obstaculo->vida;
    obstaculo->vida -= dano;
    
    if(obstaculo->vida <= 9 && vida_ant >= 10)
        obstaculo->obs_type = obstaculo->obs_type + 5;

    if (obstaculo->vida < 7 && vida_ant >= 7)
        obstaculo->obs_type = obstaculo->obs_type + 5;

    if (obstaculo->vida < 4 && vida_ant == 4)
        obstaculo->obs_type = obstaculo->obs_type + 5;

    if (obstaculo->vida == 0)
        obstaculo->obs_type = obstaculo->obs_type + 5;
}

void destroi_obstaculo(obstacle* atual, obstacle* antes){

    if(antes) antes->prox = atual->prox;
    free(atual);
}