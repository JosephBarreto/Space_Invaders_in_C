#include <stdlib.h>
#include "PowerUp.h"

shield* gera_escudo(int vida) {
	shield* novo_esc;


	if (!(novo_esc = (shield*) malloc(sizeof(novo_esc))))
		return NULL;

	novo_esc->life_shield = vida;
	novo_esc->timer_shield = 600 * vida;

	return novo_esc;
}


shotgun* gera_shotgun(int dano) {
	shotgun* novo_gun;


	if (!(novo_gun = (shotgun*) malloc(sizeof(novo_gun))))
		return NULL;

	novo_gun->shot_dano = dano;
	novo_gun->timer_shotgun = 200 * dano;

	return novo_gun;
}
