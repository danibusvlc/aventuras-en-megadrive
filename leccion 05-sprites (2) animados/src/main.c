/**
 *      @Title:  Leccion 06 - "sprites (2) animados"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#include "fondos.h"     //carga la imagen de background
#include "sprites.h"    //carga el sprite de Sonic

#define ANIM_STAND      0
#define ANIM_WAIT       1
#define ANIM_WALK       2
#define ANIM_RUN        3
#define ANIM_BRAKE      4
#define ANIM_UP         5
#define ANIM_CROUNCH    6
#define ANIM_ROLL       7

//Declaramos la funcion para recoger la entrada del mando
static void handleInput();

//Declaramos una variable (un puntero) para referirnos al sprite
Sprite* mi_sonic;

// Posicion en pantalla del sprite
u32 posx = 64;
u32 posy = 155;


int main()
{
    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //FONDO DE PANTALLA

        //para llevar la cuenta de tiles en VRAM
        u16 ind;

        //recoge la paleta del fondo
        VDP_setPalette(PAL1, fondo2.palette->data);

        //Tiles en VRAM desde la 2a pos (1er tile para pintar el fondo)
        ind = 1;

        //carga el fondo en el plano B e incrementa ind
        VDP_drawImageEx(PLAN_B, &fondo2 ,  TILE_ATTR_FULL(PAL1, 1, 0, 0, ind),  0,  0, 1, CPU);
        ind += fondo2.tileset->numTile;

    //SPRITES

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //recoje la paleta de sonic y la mete en la 1a paleta del sistema
        VDP_setPalette(PAL0,sonic_sprite.palette->data);

        //añade el sprite de Sonic
        mi_sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    //AYUDA EN PANTALLA (en el plano A)

        //antes cambiamos a una paleta con el color blanco en color ind 15 de paleta
        VDP_setTextPalette(PAL1);

        //textos
        VDP_drawText("Pulsa A  -  Sonic SIN PRIORIDAD       ", 1, 0);
        VDP_drawText("Pulsa B  -  Sonic CON PRIORIDAD       ", 1, 1);

    //Bucle principal
    while(TRUE)
    {
        //recoje la entrada de los mandos
        handleInput();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }

    return 0;
}

//Funcion handleInput()  recoje la entrada del mando (izq,dcha) y actualiza la posicion
//del sprite mi_sonic, sumando o restando de la variable posx
//además le damos la vuelta al sprite al movernos horizontalmente y cambiamos prioridad
//con A y B
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
    {
        SPR_setPosition(mi_sonic, posx--, posy);
        SPR_setHFlip(mi_sonic, TRUE);
        SPR_setAnim(mi_sonic, ANIM_RUN);
        //SPR_setVFlip(mi_sonic, TRUE);
    }

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
    {
        SPR_setPosition(mi_sonic, posx++, posy);
        SPR_setHFlip(mi_sonic, FALSE);
        SPR_setAnim(mi_sonic, ANIM_RUN);
        //SPR_setVFlip(mi_sonic, FALSE);
    }

    //si pulsamos A...
    if (value & BUTTON_A)
    {
        SPR_setPriorityAttribut(mi_sonic, FALSE);
    }
    //si pulsamos B...
    if (value & BUTTON_B)
    {
        SPR_setPriorityAttribut(mi_sonic, TRUE);
    }

    //si no pulsamos
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
    {
        SPR_setAnim(mi_sonic, ANIM_STAND);
    }
}
