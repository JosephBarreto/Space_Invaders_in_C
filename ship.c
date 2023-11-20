#include <stdlib.h>
#include "ship.h"
#include "PowerUp.h"

nave* cria_nave(unsigned char side, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y){
    nave* nova_nave;
    
    if ((x - side/2 < 0) || (x + side/2 > max_x) || (y - side/2 < 0) || (y + side/2 > max_y))
        return NULL;    

    if(!(nova_nave = (nave*) malloc(sizeof(nave))))
        return NULL;


    nova_nave->lado = side;
    nova_nave->nave_x = x;
    nova_nave->nave_y = y;
    nova_nave->nave_max_x = x+74;
    nova_nave->nave_max_y = y+46;
    nova_nave->gunTimer = 0;
    nova_nave->points = 0;
    nova_nave->vidas = 3;
    nova_nave->controle = joystick_create();
    nova_nave->shots = NULL;

    return nova_nave;
}

void move_nave(nave* ship, char passos, unsigned char direcao, unsigned short max_x, unsigned short max_y){
    /*Direcao 0 = esquerda, 1 = direita*/
    
    if(!direcao){
        if ((ship->nave_x - passos * NAVE_PASSO) - ship->lado / 2 >= 0) {
            ship->nave_x = ship->nave_x - passos * NAVE_PASSO;
            ship->nave_max_x = ship->nave_x + 74;
        }
    }
    else{
        if ((ship->nave_x + passos * NAVE_PASSO) + ship->lado / 2 < max_x - 60) {
            ship->nave_x = ship->nave_x + passos * NAVE_PASSO;
            ship->nave_max_x = ship->nave_x + 74;
        }
    }
    
}

int dano_nave(nave* ship) {

    ship->vidas -= 1;
    if (!ship->vidas) return 2;

    ship->gunTimer = 0;

    return 1;
}

void nave_atira(nave* ship, shotgun* dois_tiros){
        tiro* disparo;

    if (dois_tiros) {
        if ((disparo = cria_tiro(ship->nave_x + 3, ship->nave_y + 5, 0, 1, 0, ship->shots)))
            ship->shots = disparo;

        if ((disparo = cria_tiro(ship->nave_x + 63, ship->nave_y + 5, 0, 1, 0, ship->shots)))
            ship->shots = disparo;

        return;
    }
    if((disparo = cria_tiro(ship->nave_x+33, ship->nave_y+5, 0, 1, 0, ship->shots)))
        ship->shots = disparo;        
}

void destroi_nave(nave* ship){

    destroi_tiros(ship->shots);
    joystick_destroy(ship->controle);                                                                                                                   //Destrói o controle do quadrado
    free(ship); 

}
