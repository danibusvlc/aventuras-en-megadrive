/**
 *      @Title:  Leccion 08 - "shadow & highlight (1)"
 *      @Author: Daniel Bustos "danibus"
 */


#include <genesis.h>
#include "fondos.h"  //carga background
#include "sprite.h"  //carga sprites
#include "sys.h"     //para el hard reset

//Decl. Funciones
static void handleInput();
void myJoyHandler( u16 joy, u16 changed, u16 state);
void reset();

//Sonic sprite
Sprite* mi_sonic;

// Posicion en pantalla de Sonic
u32 mi_sonic_posx = 64;
u32 mi_sonic_posy = 155;

//Sprite barra horizontal con paleta completa
Sprite* mi_barra;

// Posicion en pantalla de la barra
u32 mi_barra_posx = 120;
u32 mi_barra_posy = 96;

//Para los estados
int estado = 0;

//variable para llevar el control de tiles
u16 ind;

int main()
{
    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //recoje la paleta de los fondos y los asigna la 1a del sistema (pal0)
    VDP_setPalette(PAL0,fondo1.palette->data);

    //carga los fondos en el VDP
    ind = TILE_USERINDEX;
    //VDP_drawImageEx(PLAN_B, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    //ind += fondo1.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;

    //recoge la paleta de sonic y la mete en la 2a paleta del sistema (pal1)
    VDP_setPalette(PAL1,sonic_sprite.palette->data);

    //recoge la paleta de la barra y la mete en la 3a y 4a paleta del sistema (pal2, pal3)
    VDP_setPalette(PAL2,barra_sprite.palette->data);
    VDP_setPalette(PAL3,barra_sprite.palette->data);

    //añade el sprite de la barra
    mi_barra = SPR_addSprite(&barra_sprite, mi_barra_posx, mi_barra_posy, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

    //añade el sprite de Sonic
    mi_sonic = SPR_addSprite(&sonic_sprite, mi_sonic_posx, mi_sonic_posy, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

    //deteccion mando
	JOY_init();
	JOY_setEventHandler( &myJoyHandler );

	//Texto inicial
    VDP_drawText("PRUEBA  0", 1, 0);
    VDP_drawText("-Pulsa A para seguir-", 1, 27);

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

// MANDO DETECCION SINCRONA
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(mi_barra, mi_barra_posx--, mi_barra_posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_barra, mi_barra_posx++, mi_barra_posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_barra, mi_barra_posx, mi_barra_posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_barra, mi_barra_posx, mi_barra_posy++);
}

// MANDO DETECCION A-SINCRONA
// Con A,B,C vamos jugando con las distintas formas de usar S/H
void myJoyHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
	    //ESTADO 0 : partimos de S/H DESACTIVADO
	    //todo 'aparece normalmente'

	    //ESTADO 01: ACTIVA S&H
	    //todo tiene NO prioridad, por tanto todo se oscurece SHADOW
        if(estado == 0 && (changed & BUTTON_A) )
        {
            estado = 1;
            VDP_drawText("PRUEBA 01", 1, 0);
            VDP_drawText("-Pulsa B para seguir-", 1, 27);
            VDP_setHilightShadow(1);
        }

	    //ESTADO 02:

        if(estado == 1 && (changed & BUTTON_B) )
        {
            estado = 2;
            VDP_drawText("PRUEBA 02", 1, 0);
            VDP_drawText("-Pulsa C para seguir-", 1, 27);
            SPR_setPriorityAttribut(mi_sonic, TRUE);
            SPR_setPriorityAttribut(mi_barra, FALSE);
        }

	    //ESTADO 03:

        if(estado == 2 && (changed & BUTTON_C) )
        {
            estado = 3;
            VDP_drawText("PRUEBA 03", 1, 0);
            VDP_drawText("-Pulsa A para seguir-", 1, 27);
            SPR_setPriorityAttribut(mi_sonic, FALSE);
            SPR_setPriorityAttribut(mi_barra, TRUE);
        }

	    //ESTADO 04:

        if(estado == 3 && (changed & BUTTON_A) )
        {
            estado = 4;
            VDP_drawText("PRUEBA 04", 1, 0);
            VDP_drawText("-Pulsa B para seguir-", 1, 27);
            SPR_setPriorityAttribut(mi_sonic, FALSE);
            SPR_setPriorityAttribut(mi_barra, FALSE);
            SPR_setPalette(mi_barra, PAL3);
        }

	    //ESTADO 05:

        if(estado == 4 && (changed & BUTTON_B) )
        {
            estado = 5;
            VDP_drawText("PRUEBA 05", 1, 0);
            VDP_drawText("-Pulsa C para seguir-", 1, 27);
            SPR_setPriorityAttribut(mi_barra, TRUE);
        }

 	    //ESTADO 06:

        if(estado == 5 && (changed & BUTTON_C) )
        {
            estado = 6;
            //volvemos a dibujar el fondo, ahora con prioridad
            ind = TILE_USERINDEX;
            VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo2.tileset->numTile;
            VDP_drawText("PRUEBA 06", 1, 0);
            VDP_drawText("-Pulsa A para seguir-", 1, 27);
            SPR_setPriorityAttribut(mi_sonic, FALSE);
            SPR_setPriorityAttribut(mi_barra, FALSE);
        }

 	    //ESTADO 07:

        if(estado == 6 && (changed & BUTTON_A) )
        {
            estado = 7;
            VDP_drawText("PRUEBA 07", 1, 0);
            VDP_drawText("-Pulsa B para seguir-", 1, 27);
            //movemos a Sonic a una posición más visible
            SPR_setPosition(mi_sonic, mi_sonic_posx-30, mi_sonic_posy);
        }

	    //ESTADO 08:

        if(estado == 7 && (changed & BUTTON_B) )
        {
            estado = 8;
            VDP_drawText("PRUEBA 08", 1, 0);
            VDP_drawText("-Pulsa C para reiniciar-", 1, 27);
            SPR_setPriorityAttribut(mi_sonic, TRUE);
            SPR_setPriorityAttribut(mi_barra, TRUE);
        }

        //reseteamos
        if(estado == 8 && (changed & BUTTON_C) )
        reset();

	}

}


// reset
void reset()
{
    SYS_hardReset();
}

