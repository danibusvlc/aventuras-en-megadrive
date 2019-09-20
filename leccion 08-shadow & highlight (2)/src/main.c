/**
 *      @Title:  Leccion 08 - "shadow & highlight (2)"
 *      @Author: Daniel Bustos "danibus"
 */


#include <genesis.h>
#include "fondos.h"  //carga fondos
#include "sprite.h"  //carga sprites

//animaciones del circulo
#define CIRCULO14   0
#define CIRCULO15   1

//Decl. Funciones
static void handleInput();
void myJoyHandler( u16 joy, u16 changed, u16 state);
void pinta_ayuda_en_pantalla();

//Sprites
Sprite* mi_sonic;
Sprite* mi_objetivo;

//Posicion en pantalla de Sonic
u32 mi_sonic_posx = 64;
u32 mi_sonic_posy = 155;

// Posicion en pantalla del objetivo
u32 mi_objetivo_posx = 120;
u32 mi_objetivo_posy = 96;

//Para activar/desactivar S/H
int sh_activo = 0;              //S&H activo (1) o no (0)
int sh_paleta = 2;              //cambia entre paleta2 (2) o paleta3 (3)
int sh_priori = 0;              //prioridad para el sprite del circulo (1) o no tenerla (0)
int sh_color  = 14;             //cambia entre el sprite del circulo pintado con el color 14 (14) o el color 15 (15)
int sh_fondo_priori = 0;        //prioridad para el fondo (1) o no tenerla (0)

//variable para llevar el control de tiles
u16 ind;

int main()
{
    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //recoje la paleta del fondo y los asigna la 1a del sistema (pal0)
    VDP_setPalette(PAL0,fondo1.palette->data);

    //carga fondo en el VDP
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;

    //recoge la paleta de sonic y la mete en la 2a paleta del sistema (pal1)
    VDP_setPalette(PAL1,sonic_sprite.palette->data);

    //recoge la paleta del objetivo y la mete en la 3a y 4a paleta del sistema (pal2, pal3)
    VDP_setPalette(PAL2,circulo_sprite.palette->data);
    VDP_setPalette(PAL3,circulo_sprite.palette->data);

    //añade el sprite del circulo
    mi_objetivo = SPR_addSprite(&circulo_sprite, mi_objetivo_posx, mi_objetivo_posy, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
    SPR_setAnim(mi_objetivo, CIRCULO14);

    //añade el sprite de Sonic
    mi_sonic = SPR_addSprite(&sonic_sprite, mi_sonic_posx, mi_sonic_posy, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    //deteccion mando
	JOY_init();
	JOY_setEventHandler( &myJoyHandler );

	//AYUDA
    VDP_drawText("S&H: OFF                                ", 0, 0);
    VDP_drawText("PALETA CIRCULO: PAL2                    ", 0, 1);
    VDP_drawText("Prioridad: NO                           ", 0, 2);
    VDP_drawText("Color circulo: 14                       ", 0, 3);
    VDP_drawText("Fondo PRIORIDAD: OFF                    ", 0, 4);
    VDP_drawText("A:SH on/off, B:Paleta, C:Prioridad Circ.", 0, 26);
    VDP_drawText("X: color14/15, Y: Prioridad Fondo       ", 0, 27);


    VDP_setHilightShadow(0);    // Highlight/shadow NO ACTIVO en el inicio

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
        SPR_setPosition(mi_objetivo, mi_objetivo_posx--, mi_objetivo_posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_objetivo, mi_objetivo_posx++, mi_objetivo_posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_objetivo, mi_objetivo_posx, mi_objetivo_posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_objetivo, mi_objetivo_posx, mi_objetivo_posy++);
}



// MANDO DETECCION A-SINCRONA
// Con A,B,C vamos jugando con las distintas formas de usar S/H
void myJoyHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
        //si pulsamos A...
        if (state & BUTTON_A)
        {
            if(sh_activo)   sh_activo = 0;
            else            sh_activo = 1;

            VDP_setHilightShadow(sh_activo);
        }

        //si pulsamos B...
        if (state & BUTTON_B)
        {
            if(sh_paleta==3)
            {
                sh_paleta = 2;
                SPR_setPalette(mi_objetivo, PAL2);
            }
            else
            {
                sh_paleta = 3;
                SPR_setPalette(mi_objetivo, PAL3);
            }
        }

        //si pulsamos C...
        if (state & BUTTON_C)
        {
            if(sh_priori)
            {
                sh_priori = 0;
                SPR_setPriorityAttribut(mi_objetivo, FALSE);
            }
            else
            {
                sh_priori = 1;
                SPR_setPriorityAttribut(mi_objetivo, TRUE);
            }
        }

        //si pulsamos X...
        if (state & BUTTON_X)
        {
            if(sh_color==14)
            {
                sh_color = 15;
                SPR_setAnim(mi_objetivo, CIRCULO15);
            }
            else
            {
                sh_color = 14;
                SPR_setAnim(mi_objetivo, CIRCULO14);
            }
        }

        //si pulsamos Y...
        //volvemos a dibujar el fondo, con y sin prioridad
        if (state & BUTTON_Y)
        {
            if(sh_fondo_priori)
            {
                sh_fondo_priori = 0;
                ind = TILE_USERINDEX;
                VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
                ind += fondo1.tileset->numTile;
            }
            else
            {
                sh_fondo_priori = 1;
                ind = TILE_USERINDEX;
                VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
                ind += fondo1.tileset->numTile;
            }
        }
	}

	pinta_ayuda_en_pantalla();
}


void pinta_ayuda_en_pantalla()
{

    if(sh_activo)       VDP_drawText("S&H: OFF                                ", 0, 0);
    else                VDP_drawText("S&H: ON                                 ", 0, 0);

    if(sh_paleta==3)    VDP_drawText("PALETA CIRCULO: PAL2                    ", 0, 1);
    else                VDP_drawText("PALETA CIRCULO: PAL3                    ", 0, 1);

    if(sh_priori)       VDP_drawText("Prioridad: NO                           ", 0, 2);
    else                VDP_drawText("Prioridad: SI                           ", 0, 2);

    if(sh_color==14)    VDP_drawText("Color circulo: 15                       ", 0, 3);
    else                VDP_drawText("Color circulo: 14                       ", 0, 3);

    if(sh_fondo_priori) VDP_drawText("Fondo PRIORIDAD: OFF                    ", 0, 4);
    else                VDP_drawText("Fondo PRIORIDAD:  ON                    ", 0, 4);

    VDP_drawText("A:SH on/off, B:Paleta, C:Prioridad Circ.", 0, 26);
    VDP_drawText("X: color14/15, Y: Prioridad Fondo       ", 0, 27);
}
