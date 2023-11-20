#include <stdlib.h>
#include <stdio.h>
#include "enemy.h"
#include "shot.h"
#include "time.h"

enemy* add_enemy(enemy* inim, int position_x, int position_y, char type, unsigned short max_x, unsigned short max_y, lista_tiros* tiros){
	enemy* n_inim, *inim_aux;

	if(position_x < 0 || position_y < 0 || position_x > max_x || position_y > max_y)
		return NULL;

	if(!(n_inim = malloc(sizeof(enemy))))
		return NULL;

	n_inim->enemy_x = position_x;
	n_inim->enemy_y = position_y;
	switch(type){
		case 1:
			n_inim->enemy_max_x = position_x+48;
		break;

		case 2:
			n_inim->enemy_max_x = position_x+66;		
		break;

		case 3:
			n_inim->enemy_max_x = position_x+72;
		break;

		default:
			n_inim->enemy_max_x = position_x+90;
	}

	n_inim->enemy_max_y = position_y+48;
	n_inim->cont_explo = 0;
	n_inim->enemy_type = type;
	n_inim->enemy_shots = tiros;
	n_inim->prox = NULL;

	if(inim){
		inim_aux = inim;
		while(inim_aux->prox)
			inim_aux = (enemy*) inim_aux->prox;
		inim_aux->prox = (struct enemy*) n_inim;
		return inim;
	}

	return n_inim;
}

int move_inimigos(enemy* inimigos, unsigned char direcao, unsigned short max_x, unsigned short max_y) {
	/*Direcao 0 = esquerda, 1 = direita*/
	enemy* aux_inim;
	int tam;

	if (!inimigos) return 1;

	aux_inim = inimigos;

	/*para baixo se for atingir a parede*/
	while (aux_inim) {

		if ((direcao && (aux_inim->enemy_max_x + 12) >= max_x) || (!direcao && ((aux_inim->enemy_x - 12) <= 0))) {
		
			for (aux_inim = inimigos; aux_inim->prox != NULL; aux_inim = (enemy*)aux_inim->prox) {


				aux_inim->enemy_y = aux_inim->enemy_y + 20;
				aux_inim->enemy_max_y = aux_inim->enemy_y + 60;

			}

			aux_inim->enemy_y = aux_inim->enemy_y + 20;
			aux_inim->enemy_max_y = aux_inim->enemy_y + 60;
			if (aux_inim->enemy_y >= max_y - 200) return 2;
			return direcao ^ 1;
		}
		aux_inim = (enemy*)aux_inim->prox;
	}


	for (aux_inim = inimigos; aux_inim != NULL; aux_inim = (enemy*)aux_inim->prox){

		switch (aux_inim->enemy_type) {
		case 1: tam = 48;
			break;

		case 2: tam = 66;
			break;

		case 3: tam = 72;
			break;

		default: tam = 90;
		}
		if (!direcao) {
			aux_inim->enemy_x = aux_inim->enemy_x - 12;
			aux_inim->enemy_max_x = aux_inim->enemy_x + tam;
			
		}
		else {
			aux_inim->enemy_x = aux_inim->enemy_x + 12;
			aux_inim->enemy_max_x = aux_inim->enemy_x + tam;
		}
	}
	
	return direcao;
}

void escolhe_atirador(enemy* inimigo) {
	enemy* aux_inim, * aux_inim2;
	tiro* inicio_tir, *aux_tir;
	int chance_atira, mesma_coluna = 0, atiraram = 0;

	if (!inimigo) return;

	inicio_tir = inimigo->enemy_shots->tiros;

	/*sorteia chances, com limite de dois tiros*/
	/*tentar atirar com inimigos 3, se tem um inimigo ou um tiro no mesmo x, não atira*/
	/*while (atiraram < 2) {*/
		for (aux_inim = inimigo; aux_inim != NULL; aux_inim = (enemy*)aux_inim->prox) {

			if (aux_inim->enemy_type == 3) {

				chance_atira = rand() % 100;
				if (chance_atira < 10 && atiraram < 2) {
					for (aux_inim2 = (enemy *) aux_inim->prox; aux_inim2 != NULL; aux_inim2 = (enemy*)aux_inim2->prox)
						if (aux_inim->enemy_x == aux_inim2->enemy_x)
							mesma_coluna = 1;

					for (aux_tir = inicio_tir; aux_tir != NULL; aux_tir = (tiro*)aux_tir->prox)
						if (aux_tir->x >= aux_inim->enemy_x && aux_tir->x <= aux_inim->enemy_max_x)
							mesma_coluna = 1;

					if (!mesma_coluna) {
						atiraram++;
						inimigo_atira(aux_inim);
					}
				}
			}
			mesma_coluna = 0;
		}
	
		for (aux_inim = inimigo; aux_inim != NULL; aux_inim = (enemy*)aux_inim->prox) {
		
			if (aux_inim->enemy_type == 2) {
		
				chance_atira = rand() % 100;
				if (chance_atira < 10 && atiraram < 2) {

					for (aux_tir = inicio_tir; aux_tir != NULL; aux_tir = (tiro*)aux_tir->prox)
						if (aux_tir->x >= aux_inim->enemy_x && aux_tir->x <= aux_inim->enemy_max_x)
							mesma_coluna = 1;

					if (!mesma_coluna) {
						atiraram++;
						inimigo_atira(aux_inim);
					}
				}
				
			}
		}

	
		for (aux_inim = inimigo; aux_inim != NULL; aux_inim = (enemy*)aux_inim->prox) {

			if (aux_inim->enemy_type == 1) {

				chance_atira = rand() % 100;
				if (chance_atira < 10 && atiraram < 2) {
					atiraram++;
					inimigo_atira(aux_inim);
				}
			}
		}
}

void inimigo_atira(enemy* inimigo) {
	tiro* disparo;

	if (!inimigo) return;
	switch(inimigo->enemy_type){
		case 1:
			if ((disparo = cria_tiro((inimigo->enemy_x + inimigo->enemy_max_x) / 2, inimigo->enemy_y + 64, 1, 2, 1, inimigo->enemy_shots->tiros)))
				inimigo->enemy_shots->tiros = disparo;
		break;
		case 2:
			if ((disparo = cria_tiro((inimigo->enemy_x + inimigo->enemy_max_x) / 2, inimigo->enemy_y + 64, 1, 2, 2, inimigo->enemy_shots->tiros)))
				inimigo->enemy_shots->tiros = disparo;
		break;
		case 3:
			if ((disparo = cria_tiro((inimigo->enemy_x + inimigo->enemy_max_x) / 2, inimigo->enemy_y + 64, 1, 1, 3, inimigo->enemy_shots->tiros)))
				inimigo->enemy_shots->tiros = disparo;
		break;
	}
}

enemy* proximo_inimigo(enemy* inicio){
	return (enemy*) inicio->prox;
}

nave_mae* gera_nave_mae(int x, int y) {
	nave_mae* mae;
	int aleat;

	if (!(mae = malloc(sizeof(nave_mae))))
		return NULL;

	mae->enemy_x = x;
	mae->enemy_y = y;
	mae->enemy_max_x = x + 102;
	mae->enemy_max_y = y + 48;
	mae->cont_explo = 0;

	aleat = rand() % 5 + 1;
	switch (aleat)
	{
		case 1:
			mae->pontua = 50;
			break;
		case 2:
			mae->pontua = 100;
			break;
		case 3:
			mae->pontua = 150;
			break;
		case 4:
			mae->pontua = 200;
			break;
		case 5:
			mae->pontua = 300;
			break;

	}
	
	return mae;
	
}

int movimenta_nave_mae(nave_mae* mae, int max_x) {
	mae->enemy_x = mae->enemy_x + 5;
	mae->enemy_max_x = mae->enemy_x + 102;

	if (mae->enemy_x >= max_x) {
		return 1;
	}
	return 0;
}

void remove_enemy(enemy* inim, enemy* anterior){

	if(anterior) anterior->prox = inim->prox;
	free(inim);
}
