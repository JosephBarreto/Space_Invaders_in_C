#ifndef __BULLET__
#define __BULLET__

#define PLAYER_TIRO_MOVE 20
#define ENEMY_TIRO_MOVE 15

typedef struct {
	unsigned short x;
	unsigned short y;
	unsigned short trajeto;    /*1 para baixo, 0 para cima*/
	unsigned short dano_tiro;
	unsigned short tipo_tiro;
	struct tiro* prox;
} tiro;

typedef struct {
	tiro* tiros;
} lista_tiros;


lista_tiros* cria_lista_compart();
tiro* cria_tiro(unsigned short x, unsigned short y, unsigned short traject, unsigned short dano, unsigned short tipo, tiro *prox);
void destroi_tiros(tiro *tiros);
void destroi_um_tiro(tiro* shot, tiro* ant);

#endif