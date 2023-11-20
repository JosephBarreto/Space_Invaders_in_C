#ifndef __OBSTACLE__
#define __OBSTACLE__

#include "shot.h"

typedef struct{
	unsigned short obs_x;
	unsigned short obs_y;
	unsigned short obs_max_x;
	unsigned short obs_max_y;
	unsigned short vida;
	unsigned char obs_type;
	struct obstacle* prox;
} obstacle;

obstacle* cria_obstaculo(int x, int y, int tipo, obstacle* lista);
obstacle* dano_obstaculo(obstacle* obstaculo, int dano);
void destroi_obstaculo(obstacle* atual, obstacle* antes);

#endif