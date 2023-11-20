#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "ship.h"
#include "enemy.h"
#include "cheat.h"
#include "obstaculo.h"
#include "PowerUp.h"
#define X_SCREEN 1280
#define Y_SCREEN 860

unsigned short verifica_colisao(nave* shooter, int x, int y, int max_x, int max_y) {
    tiro* aux = shooter->shots, * ant = NULL;
    int dano;

    while (aux) {

        if ((aux->y >= y) && (aux->y <= max_y) &&
            (aux->x >= x) && (aux->x <= max_x)) {
            if (!ant) shooter->shots = (tiro*)aux->prox;
            dano = aux->dano_tiro;
            destroi_um_tiro(aux, ant);
            return dano;
        }
        ant = aux;
        aux = (tiro*)aux->prox;
    }

    return 0;
}

unsigned short verifica_colisao_ataque(enemy* shooter, int x, int y, int max_x, int max_y) {
    tiro* aux, * ant = NULL;
    int dano;
    if (!shooter) return 0;
    aux = shooter->enemy_shots->tiros;
    while (aux) {

        if ((aux->y >= y) && (aux->y <= max_y) &&
            (aux->x >= x) && (aux->x <= max_x)) {
            if (!ant) shooter->enemy_shots->tiros = (tiro*)aux->prox;
            dano = aux->dano_tiro;
            destroi_um_tiro(aux, ant);
            return dano;
        }
        ant = aux;
        aux = (tiro*)aux->prox;
    }

    return 0;
}

void checa_obstaculo(nave* ship, enemy* inim, obstacle* obstac) {
    obstacle* aux_obstac;
    int dano;

    aux_obstac = obstac;
    while (aux_obstac) {

        if (dano = verifica_colisao(ship, aux_obstac->obs_x, aux_obstac->obs_y,
            aux_obstac->obs_max_x, aux_obstac->obs_max_y) ||
            (dano = verifica_colisao_ataque(inim, aux_obstac->obs_x, aux_obstac->obs_y,
                aux_obstac->obs_max_x, aux_obstac->obs_max_y)))
            dano_obstaculo(aux_obstac, dano);
        aux_obstac = (obstacle*)aux_obstac->prox;
    }
}

int checa_dano_nave(nave* ship, enemy* shooter, shield* escudo) {

    if (verifica_colisao_ataque(shooter, ship->nave_x, ship->nave_y-15, ship->nave_max_x, ship->nave_max_y-15)) {
        if (escudo) {
            escudo->life_shield--;
            return 0;
        }
        return dano_nave(ship);
    }
    return 0;
}

int checa_morte_navemae(nave* ship, nave_mae* mae) {
    nave_mae* aux_mae = NULL;

    aux_mae = mae;
    if (verifica_colisao(ship, aux_mae->enemy_x, aux_mae->enemy_y,
        aux_mae->enemy_max_x, aux_mae->enemy_max_y)) {
        ship->points += aux_mae->pontua;
        aux_mae->cont_explo = 1;
        return 1;
    }
    return 0;
}

void checa_morte_inimigos(nave* ship, enemy* lista_inim) {
    enemy* aux_inim, * ant_inim = NULL;

    aux_inim = lista_inim;
    while (aux_inim) {

        if (verifica_colisao(ship, aux_inim->enemy_x, aux_inim->enemy_y,
            aux_inim->enemy_max_x, aux_inim->enemy_max_y)) {

            switch (aux_inim->enemy_type) {
            case 1: ship->points = ship->points + 40;
                break;

            case 2: ship->points = ship->points + 20;
                break;

            case 3: ship->points = ship->points + 10;
                break;

            }
            aux_inim->enemy_type = 4;
        }
        ant_inim = aux_inim;
        aux_inim = (enemy*)aux_inim->prox;
    }
}

void update_bullets(nave* player, enemy* inimigos) {
    tiro* antes = NULL, * en_ant = NULL, * atual, * en_atu;


    for (atual = player->shots; atual != NULL;) {
        atual->y -= PLAYER_TIRO_MOVE;

        if ((atual->y < 0) || (atual->y > Y_SCREEN - 70)) {
            if (antes) {
                destroi_um_tiro(atual, antes);
                atual = (tiro*)antes->prox;
            }
            else {
                player->shots = (tiro*)atual->prox;
                destroi_um_tiro(atual, antes);
                atual = player->shots;
            }
        }
        else {
            antes = atual;
            atual = (tiro*)atual->prox;
        }
    }
    if (!inimigos) return;
    for (en_atu = inimigos->enemy_shots->tiros; en_atu != NULL;) {
        en_atu->y += ENEMY_TIRO_MOVE;

        if ((en_atu->y < 0) || (en_atu->y > Y_SCREEN - 70)) {
            if (en_ant) {
                destroi_um_tiro(en_atu, en_ant);
                en_atu = (tiro*)en_ant->prox;
            }
            else {
                inimigos->enemy_shots->tiros = (tiro*)en_atu->prox;
                destroi_um_tiro(en_atu, en_ant);
                en_atu = inimigos->enemy_shots->tiros;
            }
        }
        else {
            en_ant = en_atu;
            en_atu = (tiro*)en_atu->prox;
        }

    }
}

void update_position(nave* player, shotgun* shotg) {
    if (player->controle->left)
        move_nave(player, 2, 0, X_SCREEN, Y_SCREEN);
    if (player->controle->right)
        move_nave(player, 2, 1, X_SCREEN, Y_SCREEN);

    if (player->controle->fire) {
        if ((!player->gunTimer) && ((!player->shots) || (shotg && !player->shots->prox))) {
            nave_atira(player, shotg);
            player->gunTimer = PLAYER_COOLDOWN;
        }
    }
}

int main() {

    nave* player;
    char score[10], next_round[10], pontoMae[10];
    lista_tiros* enemyshotlist;
    codigo* trapaca = NULL;
    shield* escudo = NULL;
    shotgun* espingarda = NULL;
    obstacle* obstaculos = NULL, * aux_ob, * ant_ob;
    nave_mae* n_mae = NULL;
    enemy* inim = NULL, * inim_aux, * ant_inim = NULL;
    int tipo, cont_inim, anim, cheats, espaco_vid, start = 0, new_round = 0, round = 1,
        difficulty = 10, time_count = 0, player_hurt = 0, hurt_anim = 0, inim_direc = 1,
        beginning_x = X_SCREEN / 7 - 10, limit_x = (X_SCREEN - X_SCREEN / 6),
        beginning_y = Y_SCREEN / 5, limit_y = (Y_SCREEN / 2) + 20, shield_break = 0, maeRand = 0, death = 0;
    float divisor;

    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    srand(time(NULL));

    al_install_keyboard();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_FONT* big_font = al_load_font("bitmaps/Space_font.ttf", 40, 0);
    ALLEGRO_FONT* small_font = al_load_font("bitmaps/Space_font.ttf", 20, 0);
    ALLEGRO_DISPLAY* disp = al_create_display(X_SCREEN, Y_SCREEN);
    ALLEGRO_BITMAP* l_Cannon = al_load_bitmap("bitmaps/Laser_Cannon.bmp");
    ALLEGRO_BITMAP* line = al_load_bitmap("bitmaps/Line.bmp");
    ALLEGRO_BITMAP* i_forte = al_load_bitmap("bitmaps/Strong.bmp");
    ALLEGRO_BITMAP* i_medio = al_load_bitmap("bitmaps/Medium.bmp");
    ALLEGRO_BITMAP* i_fraco = al_load_bitmap("bitmaps/Weak.bmp");
    ALLEGRO_BITMAP* i_forte2 = al_load_bitmap("bitmaps/Strong2.bmp");
    ALLEGRO_BITMAP* i_medio2 = al_load_bitmap("bitmaps/Medium2.bmp");
    ALLEGRO_BITMAP* i_fraco2 = al_load_bitmap("bitmaps/Weak2.bmp");
    ALLEGRO_BITMAP* obstac_Fl = al_load_bitmap("bitmaps/Full_ob.bmp");
    ALLEGRO_BITMAP* obstac_LD = al_load_bitmap("bitmaps/LeftDown_ob.bmp");
    ALLEGRO_BITMAP* obstac_LU = al_load_bitmap("bitmaps/LeftUp_ob.bmp");
    ALLEGRO_BITMAP* obstac_RD = al_load_bitmap("bitmaps/RighDown_ob.bmp");
    ALLEGRO_BITMAP* obstac_RU = al_load_bitmap("bitmaps/RighUp_ob.bmp");
    ALLEGRO_BITMAP* obstac_Fl_dmg1 = al_load_bitmap("bitmaps/Full_ob_dmg1.bmp");
    ALLEGRO_BITMAP* obstac_LD_dmg1 = al_load_bitmap("bitmaps/LeftDown_ob_dmg1.bmp");
    ALLEGRO_BITMAP* obstac_LU_dmg1 = al_load_bitmap("bitmaps/LeftUp_ob_dmg1.bmp");
    ALLEGRO_BITMAP* obstac_RD_dmg1 = al_load_bitmap("bitmaps/RighDown_ob_dmg1.bmp");
    ALLEGRO_BITMAP* obstac_RU_dmg1 = al_load_bitmap("bitmaps/RighUp_ob_dmg1.bmp");
    ALLEGRO_BITMAP* obstac_Fl_dmg2 = al_load_bitmap("bitmaps/Full_ob_dmg2.bmp");
    ALLEGRO_BITMAP* obstac_LD_dmg2 = al_load_bitmap("bitmaps/LeftDown_ob_dmg2.bmp");
    ALLEGRO_BITMAP* obstac_LU_dmg2 = al_load_bitmap("bitmaps/LeftUp_ob_dmg2.bmp");
    ALLEGRO_BITMAP* obstac_RD_dmg2 = al_load_bitmap("bitmaps/RighDown_ob_dmg2.bmp");
    ALLEGRO_BITMAP* obstac_RU_dmg2 = al_load_bitmap("bitmaps/RighUp_ob_dmg2.bmp");
    ALLEGRO_BITMAP* obstac_Fl_dmg3 = al_load_bitmap("bitmaps/Full_ob_dmg3.bmp");
    ALLEGRO_BITMAP* obstac_LD_dmg3 = al_load_bitmap("bitmaps/LeftDown_ob_dmg3.bmp");
    ALLEGRO_BITMAP* obstac_LU_dmg3 = al_load_bitmap("bitmaps/LeftUp_ob_dmg3.bmp");
    ALLEGRO_BITMAP* obstac_RD_dmg3 = al_load_bitmap("bitmaps/RighDown_ob_dmg3.bmp");
    ALLEGRO_BITMAP* obstac_RU_dmg3 = al_load_bitmap("bitmaps/RighUp_ob_dmg3.bmp");
    ALLEGRO_BITMAP* explode = al_load_bitmap("bitmaps/Explosao.bmp");
    ALLEGRO_BITMAP* bullet_1 = al_load_bitmap("bitmaps/bullet1.bmp");
    ALLEGRO_BITMAP* bullet_2 = al_load_bitmap("bitmaps/bullet2.bmp");
    ALLEGRO_BITMAP* bullet_3 = al_load_bitmap("bitmaps/bullet3.bmp");
    ALLEGRO_BITMAP* ufo = al_load_bitmap("bitmaps/nave_mae.bmp");
    ALLEGRO_BITMAP* pla_dmg1 = al_load_bitmap("bitmaps/Laser_Cannondmg1.bmp");
    ALLEGRO_BITMAP* pla_dmg2 = al_load_bitmap("bitmaps/Laser_Cannondmg2.bmp");



    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    if (!(player = cria_nave(20, X_SCREEN / 6, Y_SCREEN - 100, X_SCREEN, Y_SCREEN)))
        return 1;


    for (int o = 0; o <= 3; o++) {
        switch (o) {
        case 0: divisor = 6;
            break;

        case 1: divisor = 2.8;
            break;

        case 2: divisor = 1.7;
            break;

        case 3: divisor = 1.3;
            break;

        }

        if (!obstaculos)obstaculos = cria_obstaculo(X_SCREEN / divisor, Y_SCREEN - (Y_SCREEN / 4 - 44), 1, obstaculos);
        else cria_obstaculo(X_SCREEN / divisor, Y_SCREEN - (Y_SCREEN / 4 - 44), 1, obstaculos);

        cria_obstaculo(X_SCREEN / divisor + 69, Y_SCREEN - (Y_SCREEN / 4 - 44), 1, obstaculos);
        cria_obstaculo(X_SCREEN / divisor, Y_SCREEN - (Y_SCREEN / 4 - 22), 1, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 23, Y_SCREEN - (Y_SCREEN / 4 - 22), 2, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 46, Y_SCREEN - (Y_SCREEN / 4 - 22), 3, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 69, Y_SCREEN - (Y_SCREEN / 4 - 22), 1, obstaculos);
        cria_obstaculo(X_SCREEN / divisor, Y_SCREEN - (Y_SCREEN / 4), 4, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 23, Y_SCREEN - (Y_SCREEN / 4), 1, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 46, Y_SCREEN - (Y_SCREEN / 4), 1, obstaculos);
        cria_obstaculo(X_SCREEN / divisor + 69, Y_SCREEN - (Y_SCREEN / 4), 5, obstaculos);

    }

    enemyshotlist = cria_lista_compart();
    tipo = 1;
    cont_inim = 0;
    for (int i = Y_SCREEN / 5; i <= (Y_SCREEN / 2) + 20; i = i + 60) {
        for (int j = X_SCREEN / 7 - 10; j <= (X_SCREEN - X_SCREEN / 6); j = j + 88) {
            switch (tipo) {
            case 1: inim = add_enemy(inim, j + 12, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                break;

            case 2: inim = add_enemy(inim, j + 4, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                break;

            case 3: inim = add_enemy(inim, j, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                break;

            }
        }
        cont_inim = cont_inim ^ 1;
        if (cont_inim) tipo++;
    }
    cont_inim = 0;
    trapaca = cria_codigos();

    anim = 0;
    cheats = 0;
    ALLEGRO_EVENT evento;
    al_start_timer(timer);
    while (1) {

        al_wait_for_event(queue, &evento);
        if (!start) {
            al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 45, Y_SCREEN / 2 - 50, 0, "SPACE");
            al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 74, Y_SCREEN / 2 - 20, 0, "INVADERS ");
            al_draw_text(small_font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 80, Y_SCREEN / 2 + 60, 0, "Press Space to Start");
            al_flip_display();
            if ((evento.type == 12) && (evento.keyboard.keycode == 75)) start = 1;
            else if (evento.type == 42) break;
        }
        else if (new_round) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            sprintf(next_round, "%d", round);
            al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 90, Y_SCREEN / 2 - 100, 0, "VICTORY!");

            al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 125, Y_SCREEN / 2 - 70, 0, "ROUND");
            if (round < 10) al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 + 2, Y_SCREEN / 2 - 70, 0, next_round);
            else al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 5, Y_SCREEN / 2 - 70, 0, next_round);
            al_draw_text(big_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 + 45, Y_SCREEN / 2 - 70, 0, "NEXT");

            if (round > 10) {
                al_draw_text(big_font, al_map_rgb(255, 255, 0), X_SCREEN / 2 - 220, Y_SCREEN / 2 - 35, 0, "YOU DID THE IMPOSSIBLE!");
                al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 113, Y_SCREEN / 2 + 5, 0, "You gained 10000 Points!");
            }
            else al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 113, Y_SCREEN / 2 - 25, 0, "You gained 1000 Points!");

            if (round <= 7) cont_inim = 0;
            else cont_inim = 1;
            switch (round)
            {
            case 3:
                difficulty = 7;
                al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 125, Y_SCREEN / 2 + 2, 0, "Stronger Enemys incoming!");
                break;

            case 5:
                beginning_x = X_SCREEN / 9 - 10;
                limit_x = X_SCREEN - (X_SCREEN / 9);
                al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 142, Y_SCREEN / 2 + 2, 0, "More Stronger Enemys incoming!");
                break;
            case 7:
                difficulty = 5;
                cont_inim = 1;
                beginning_x = X_SCREEN / 11 - 10;
                limit_x = X_SCREEN - (X_SCREEN / 11);
                beginning_y = (Y_SCREEN / 5) - 20;
                limit_y = (Y_SCREEN / 2) + 100;
                al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 180, Y_SCREEN / 2 + 2, 0, "More and Way Stronger Enemys incoming!");
                break;
            case 10:
                difficulty = 3;
                al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 2 - 135, Y_SCREEN / 2 + 2, 0, "STRONGEST Enemys incoming!!!");
                break;
            }
            if(player->vidas == 5) al_draw_text(small_font, al_map_rgb(0, 255, 255), X_SCREEN / 2 + 55, Y_SCREEN - 300, 0, "You have 5 lives so you gained a Shield!");
            al_draw_text(small_font, al_map_rgb(255, 255, 255), X_SCREEN / 2 - 115, Y_SCREEN / 2 + 30, 0, "Press Space to Continue");
            al_flip_display();
            if ((evento.type == 12) && (evento.keyboard.keycode == 75)) {

                if (player->vidas < 5) player->vidas++;
                else {
                    if (!escudo) escudo = gera_escudo(1);
                    else escudo = gera_escudo(escudo->life_shield + 1);
                }
                if (round > 10) player->points += 10000;
                else player->points += 1000;
                new_round = 0;
            }
            else if (evento.type == 42) break;

        }
        else if (!inim) {
            destroi_tiros(enemyshotlist->tiros);
            enemyshotlist = cria_lista_compart();
            player->controle = joystick_create();


            tipo = 1;
            for (int i = beginning_y; i <= limit_x; i = i + 60) {
                for (int j = beginning_x; j <= limit_x; j = j + 88) {
                    switch (tipo) {
                    case 1: inim = add_enemy(inim, j + 12, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                        break;

                    case 2: inim = add_enemy(inim, j + 4, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                        break;

                    case 3: inim = add_enemy(inim, j, i, tipo, X_SCREEN, Y_SCREEN, enemyshotlist);
                        break;

                    }
                }
                cont_inim = cont_inim ^ 1;
                if (cont_inim) tipo++;
            }
            time_count = 0;

        }
        else if (evento.type == 30 && player_hurt) {
            
            if (!(al_get_timer_count(timer) % 10)) {
                al_draw_filled_rectangle(player->nave_x, player->nave_y, player->nave_x + 76, player->nave_y + 50, al_map_rgb(0, 0, 0));
                al_draw_bitmap(pla_dmg1, player->nave_x, player->nave_y, 0);
            }
            if (!(al_get_timer_count(timer) % 20)) {
                al_draw_filled_rectangle(player->nave_x, player->nave_y, player->nave_x + 76, player->nave_y + 50, al_map_rgb(0, 0, 0));
                al_draw_bitmap(pla_dmg2, player->nave_x, player->nave_y, 0);
            }
            al_flip_display();
            hurt_anim++;
            if (hurt_anim > 69) {
                al_draw_filled_rectangle(player->nave_x, player->nave_y, player->nave_x + 76, player->nave_y + 50, al_map_rgb(0, 0, 0));
                tiro* aux_tiro = inim->enemy_shots->tiros;

                if (aux_tiro) inim->enemy_shots->tiros = (tiro*)aux_tiro->prox;
                destroi_um_tiro(aux_tiro, NULL);

                time_count = 0;
                player->controle = joystick_create();
                player->nave_x = X_SCREEN / 6;
                player->nave_y = Y_SCREEN - 100;
                player->nave_max_x = player->nave_x + 74;
                player->nave_max_y = player->nave_y + 46;
                hurt_anim = 0;
                player_hurt = 0;
            }
        }
        else if (evento.type == 30 && death) {

            al_draw_filled_rectangle(X_SCREEN / 3, Y_SCREEN / 3 - 10, X_SCREEN / 2 + 200, Y_SCREEN / 2 + 160, al_map_rgb(0, 0, 0));
            al_draw_text(big_font, al_map_rgb(255, 0, 0), X_SCREEN / 2 - 97, Y_SCREEN / 3 + 40, 0, "YOU DIED!");
            al_draw_text(big_font, al_map_rgb(255, 0, 0), X_SCREEN / 3 + 50, Y_SCREEN / 2 + 10, 0, "Press  X  to exit");
            al_flip_display();
            
            if (evento.type == 42) break;
        }
        else if (evento.type == 30) {
            time_count++;
            update_position(player, espingarda);
            update_bullets(player, inim);
            if (time_count >= 30 && !(al_get_timer_count(timer) % (difficulty * 5))) escolhe_atirador(inim);
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(line, 20, Y_SCREEN - (Y_SCREEN / 30), 0);
            al_draw_bitmap(l_Cannon, player->nave_x, player->nave_y, 0);

            /*trapaças*/
            if (cheats == 11) {
                if (!espingarda) espingarda = gera_shotgun(3);
                else {
                    espingarda->shot_dano = 3;
                    espingarda->timer_shotgun = 600 * 3;
                }
                cheats = 0;
            }
            if (cheats == 12) {
                if (!escudo) escudo = gera_escudo(999999);
                else {
                    escudo->life_shield = 999999;
                    escudo->timer_shield = 600 *999999;
                }
                cheats = 0;
            }
            if (cheats == 13) {
                inim_aux = inim;
                while (inim) {

                    if (!inim_aux) break;
                    switch (inim_aux->enemy_type) {
                    case 1: player->points = player->points + 40;
                        break;

                    case 2: player->points = player->points + 20;
                        break;

                    case 3: player->points = player->points + 10;
                        break;

                    }
                    inim_aux->enemy_type = 4;

                    inim_aux = (enemy*)inim_aux->prox;

                }
                cheats = 0;
            }

            if (espingarda) {

                if (espingarda->timer_shotgun) {
                    espingarda->timer_shotgun--;
                 
                    al_draw_filled_rectangle(player->nave_x + 28, player->nave_y + 6, player->nave_x + 47, player->nave_y + 16, al_map_rgb(255, 0, 0));

                    if (espingarda->timer_shotgun <= 120 && ((al_get_timer_count(timer) % 30) <= 15))
                        al_draw_filled_rectangle(player->nave_x + 28, player->nave_y + 6, player->nave_x + 47, player->nave_y + 16, al_map_rgb(100, 0, 0));
                    
                }
                else {
                    free(espingarda);
                    espingarda = NULL;
                }
            }

            if (escudo) {
                if (escudo->timer_shield && escudo->life_shield > 0) {
                    escudo->timer_shield--;

                    al_draw_filled_rectangle(player->nave_x + 20, player->nave_y - 15, player->nave_x + 56, player->nave_y - 10, al_map_rgb(0, 255, 255));
                    al_draw_filled_rectangle(player->nave_x - 12, player->nave_y - 10, player->nave_x + 88, player->nave_y - 5, al_map_rgb(0, 255, 255));

                    if (escudo->timer_shield <= 120 && ((al_get_timer_count(timer) % 30) <= 15)) {
                        al_draw_filled_rectangle(player->nave_x + 20, player->nave_y - 15, player->nave_x + 56, player->nave_y - 10, al_map_rgb(0, 100, 255));
                        al_draw_filled_rectangle(player->nave_x - 12, player->nave_y - 10, player->nave_x + 88, player->nave_y - 5, al_map_rgb(0, 100, 255));
                    }
                }
                else {
                    shield_break++;
                    al_draw_filled_rectangle(player->nave_x + 20, player->nave_y - 15, player->nave_x + 56, player->nave_y - 10, al_map_rgb(0, 100, 255));
                    al_draw_filled_rectangle(player->nave_x - 12, player->nave_y - 10, player->nave_x + 88, player->nave_y - 5, al_map_rgb(0, 100, 255));
                    if (shield_break == 10) {
                        free(escudo);
                        escudo = NULL;
                        shield_break = 0;
                    }
                }
            }
            
            if (!n_mae) {
                maeRand = rand() % 1000;
                if(maeRand < 2 && !espingarda) n_mae = gera_nave_mae(0, 100);
            }
            else {

                al_draw_bitmap(ufo, n_mae->enemy_x, n_mae->enemy_y, 0);
                if (n_mae->cont_explo) {
                    sprintf(pontoMae, "%d", n_mae->pontua);

                    al_draw_filled_rectangle(n_mae->enemy_x, n_mae->enemy_y, n_mae->enemy_max_x, n_mae->enemy_max_y, al_map_rgb(0, 0, 0));
                    al_draw_text(big_font, al_map_rgb(255, 255, 255), n_mae->enemy_x+5, n_mae->enemy_y+3, 0, pontoMae);
                    n_mae->cont_explo++;
                }
                if (movimenta_nave_mae(n_mae, X_SCREEN) || n_mae->cont_explo == 10) {
                    free(n_mae);
                    n_mae = NULL;
                }
            }

            for (aux_ob = obstaculos; aux_ob != NULL; aux_ob = (obstacle*)aux_ob->prox) {
                switch (aux_ob->obs_type) {
                case 1:
                    al_draw_bitmap(obstac_Fl, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 2:
                    al_draw_bitmap(obstac_LD, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 3:
                    al_draw_bitmap(obstac_RD, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 4:
                    al_draw_bitmap(obstac_LU, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 5:
                    al_draw_bitmap(obstac_RU, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 6:
                    al_draw_bitmap(obstac_Fl_dmg1, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 7:
                    al_draw_bitmap(obstac_LD_dmg1, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 8:
                    al_draw_bitmap(obstac_RD_dmg1, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 9:
                    al_draw_bitmap(obstac_LU_dmg1, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 10:
                    al_draw_bitmap(obstac_RU_dmg1, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 11:
                    al_draw_bitmap(obstac_Fl_dmg2, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 12:
                    al_draw_bitmap(obstac_LD_dmg2, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 13:
                    al_draw_bitmap(obstac_RD_dmg2, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 14:
                    al_draw_bitmap(obstac_LU_dmg2, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 15:
                    al_draw_bitmap(obstac_RU_dmg2, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 16:
                    al_draw_bitmap(obstac_Fl_dmg3, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 17:
                    al_draw_bitmap(obstac_LD_dmg3, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 18:
                    al_draw_bitmap(obstac_RD_dmg3, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 19:
                    al_draw_bitmap(obstac_LU_dmg3, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;

                case 20:
                    al_draw_bitmap(obstac_RU_dmg3, aux_ob->obs_x, aux_ob->obs_y, 0);
                    break;
                }
            }

            /*Converte pontos atuais para uma string*/
            sprintf(score, "%d", player->points);

            if (player->shots) {
                checa_morte_inimigos(player, inim);
                if (n_mae && checa_morte_navemae(player, n_mae)) espingarda = gera_shotgun(3); 
            }
            if (inim && (inim->enemy_shots->tiros || player->shots))
                checa_obstaculo(player, inim, obstaculos);


            if (!(al_get_timer_count(timer) % (difficulty * 2)) && ((inim_direc = move_inimigos(inim, inim_direc, X_SCREEN, Y_SCREEN)) == 2)) {
                player_hurt = 1;
                player->vidas = 0;
            }


            for (inim_aux = inim; inim_aux != NULL; inim_aux = (enemy*)inim_aux->prox) {
                switch (inim_aux->enemy_type)
                {
                case 1:
                    if (anim)
                        al_draw_bitmap(i_forte, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    else
                        al_draw_bitmap(i_forte2, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    break;

                case 2:
                    if (anim)
                        al_draw_bitmap(i_medio, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    else
                        al_draw_bitmap(i_medio2, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    break;

                case 3:
                    if (anim)
                        al_draw_bitmap(i_fraco, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    else
                        al_draw_bitmap(i_fraco2, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    break;

                case 4:
                    if (inim_aux->cont_explo < 10) inim_aux->cont_explo++;
                    al_draw_bitmap(explode, inim_aux->enemy_x, inim_aux->enemy_y, 0);
                    break;
                }
            }
            inim_aux = inim;
            ant_inim = NULL;
            while (inim_aux) {
                if (inim_aux->enemy_type == 4 && inim_aux->cont_explo == 10) {
                    if (!ant_inim) inim = (enemy*)inim->prox;
                    remove_enemy(inim_aux, ant_inim);
                    inim_aux = ant_inim;
                    if (!ant_inim) inim_aux = inim;
                }
                ant_inim = inim_aux;
                if (inim_aux) inim_aux = (enemy*)inim_aux->prox;
            }
            aux_ob = obstaculos;
            ant_ob = NULL;
            while (aux_ob) {

                if (aux_ob->obs_type > 20) {
                    if (!ant_ob) obstaculos = (obstacle*)obstaculos->prox;
                    destroi_obstaculo(aux_ob, ant_ob);
                    aux_ob = ant_ob;
                    if (!ant_ob) aux_ob = obstaculos;
                }
                ant_ob = aux_ob;
                aux_ob = (obstacle*)aux_ob->prox;

            }
            for (tiro* index = player->shots; index != NULL; index = (tiro*)index->prox)
                al_draw_filled_rectangle(index->x, index->y, index->x + 5, index->y - 12, al_map_rgb(255, 255, 255));
            if (inim && inim->enemy_shots) for (tiro* index_inim = inim->enemy_shots->tiros; index_inim != NULL; index_inim = (tiro*)index_inim->prox)
                switch (index_inim->tipo_tiro) {
                case 1: al_draw_bitmap(bullet_1, index_inim->x, index_inim->y, 0);
                    break;

                case 2: al_draw_bitmap(bullet_2, index_inim->x, index_inim->y, 0);
                    break;

                case 3: al_draw_bitmap(bullet_3, index_inim->x, index_inim->y, 0);
                    break;

                }
            if (player->gunTimer) player->gunTimer--;

            /*Textos*/
            al_draw_text(small_font, al_map_rgb(255, 255, 255), X_SCREEN / 10, 20, 0, "SCORE");
            al_draw_text(small_font, al_map_rgb(0, 255, 0), X_SCREEN / 5, 20, 0, score);
            al_draw_text(small_font, al_map_rgb(255, 255, 255), X_SCREEN - X_SCREEN / 3, 20, 0, "LIVES");

            switch (checa_dano_nave(player, inim, escudo)) {
            case 1:
                player_hurt = 1;
                break;
            case 2:
                player_hurt = 1;
                break;
            }

            espaco_vid = 80;
            for (int vid = 1; vid <= player->vidas; vid++) {

                if (vid <= 3)
                    al_draw_bitmap(l_Cannon, (X_SCREEN - X_SCREEN / 3) + espaco_vid, 3, 0);
                else {
                    if (vid == 4) espaco_vid = 80;
                    al_draw_bitmap(l_Cannon, (X_SCREEN - X_SCREEN / 3) + espaco_vid, 55, 0);
                }
                espaco_vid = espaco_vid + 100;
            }

            al_flip_display();
            
            if (!player_hurt && !player->vidas) death = 1;            

            if (!(al_get_timer_count(timer) % (difficulty * 2))) anim = anim ^ 1;
            if (!inim) {
                new_round++;
                round++;
            }
        }
        else if ((evento.type == 10) || (evento.type == 12)) {
            if (evento.keyboard.keycode == 82) joystick_left(player->controle);
            else if (evento.keyboard.keycode == 83) joystick_right(player->controle);
            else if (evento.keyboard.keycode == 75) joystick_fire(player->controle);

            if (evento.type == 12) cheats = le_codigo(trapaca, cheats, evento.keyboard.keycode);
        }
        else if (evento.type == 42) break;
    }


    al_destroy_font(big_font);
    al_destroy_font(small_font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    destroi_nave(player);

    return 0;
}
