#ifndef __ENEMY__
#define __ENEMY__

#include "shot.h"

typedef struct{
	unsigned short enemy_x;
	unsigned short enemy_y;
	unsigned short enemy_max_x;
	unsigned short enemy_max_y;
	unsigned short cont_explo;
	unsigned char enemy_type;
	struct enemy* prox;
	lista_tiros* enemy_shots;
} enemy;

typedef struct {
	unsigned short enemy_x;
	unsigned short enemy_y;
	unsigned short enemy_max_x;
	unsigned short enemy_max_y;
	unsigned short cont_explo;
	unsigned short pontua;
} nave_mae;

enemy* add_enemy(enemy* inim, int position_x, int position_y, char type, unsigned short max_x, unsigned short max_y, lista_tiros* tiros);
int move_inimigos(enemy* inimigos, unsigned char direcao, unsigned short max_x, unsigned short max_y);
void escolhe_atirador(enemy* inimigo);
void inimigo_atira(enemy* inimigo);
enemy* proximo_inimigo(enemy* inicio);

nave_mae* gera_nave_mae(int x, int y);
int movimenta_nave_mae(nave_mae* mae, int max_x);
void remove_enemy(enemy* inim, enemy* anterior);

#endif