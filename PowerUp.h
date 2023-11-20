#ifndef __POWER__
#define __POWER__

typedef struct {
	int life_shield;
	int timer_shield;

} shield;

typedef struct {
	unsigned short shot_dano;
	unsigned short timer_shotgun;

} shotgun;

shield* gera_escudo(int vida);
shotgun* gera_shotgun(int dano);

#endif
