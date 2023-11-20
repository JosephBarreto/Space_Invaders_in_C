#ifndef __SHIP__
#define __SHIP__

#include "Joystick.h"
#include "shot.h"
#include "PowerUp.h"

#define NAVE_PASSO 8
#define PLAYER_COOLDOWN 5

typedef struct {
    unsigned char lado;
    unsigned short nave_x;
    unsigned short nave_y;
    unsigned short nave_max_x;
    unsigned short nave_max_y;
    unsigned short gunTimer;
    unsigned short vidas;
    int points;
    joystick* controle;
    tiro* shots;
} nave;

nave* cria_nave(unsigned char side, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y);
void move_nave(nave* ship, char passos, unsigned char direcao, unsigned short max_x, unsigned short max_y);
int dano_nave(nave* ship);
void nave_atira(nave* ship, shotgun* dois_tiros);
void destroi_nave(nave* ship);

#endif
