/**
 *      @Title:  "leccion 09 - "scroll (0) por planos"
 *      @Author: Daniel Bustos "danibus"
 */


/*
    ESTADO 1: Estado inicial. Mov del plano A en horizontal
    ESTADO 2: Mov del plano A en horizontal y vertical
    ESTADO 3: Mov del plano B en horiz+verti, plano A fijo.
    ESTADO 4: Mov de planos A y B en todas direcciones.
    ESTADO 5: Mov de planos A y B solo en horiz. Sprite fijo.
    ESTADO 6: Mov de planos A, B y Sprite (limitado a zona visible).
*/

#include <genesis.h>

#include "gfx.h"     //carga las 2 imágenes de background
#include "sprite.h"  //carga los sprites (sonic)
#include "sys.h"     //para el reset

#define ANIM_STAND      0       //animaciones de Sonic
#define ANIM_RUN        3

#define LIMITE_IZQ  20          //limites pantalla para el sprite
#define LIMITE_DCH  260


//Declaracion de funciones
static void handleInput();
static void cargaESTADO();
static void muestraMENSAJES();
static void actualizaCamara();

//Puntero a sprites
Sprite* Sonic;

//Para ir viendo los distintos ejemplos
int estado = 1;
int cerrojo = 0;

//cuenta de tiles en VRAM
u16 ind;

//desplazamiento respecto al punto (0,0) de los planos
s16 offset_H_PlanoA = 0;
s16 offset_V_PlanoA = 0;
s16 offset_H_PlanoB = 0;
s16 offset_V_PlanoB = 0;

//para ir incrementando el movimiento de los planos
int aceleracionPlanoA = 2;
int aceleracionPlanoB = 1;

//movimiento de Sonic
int aceleracionSonic = 3;
s16 posx = 48;          //POSICION de Sonic
s16 posy = 158;



int main()
{
    //320x224px
    VDP_setScreenWidth320();

    //paletas
    VDP_setPalette(PAL0, bgb_image.palette->data);
    VDP_setPalette(PAL1, bga_image.palette->data);
    VDP_setPalette(PAL2, sonic_sprite.palette->data);

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //backgrounds
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;

    //Configura el scroll (por planos)
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    //bucle ppal
    while(TRUE)
    {
        //En cada ejemplo llamamos a cargaESTADO() para inicializar cada ejemplo correctamente
        cargaESTADO();

        //bucle secundario, se abre (cerrojo=0) cada vez que cambiamos de ejemplo=estado
        //al iniciarse un ejemplo, se cierra (cerrojo=1)
        while(cerrojo)
        {
            //Controles (sincronos)
            handleInput();

            //muestra Mensajes al usuario
            muestraMENSAJES();

            //actualiza Camara
            actualizaCamara();

            //actualiza VDP
            SPR_update();

            //Espera al barrido vertical TV
            VDP_waitVSync();
        }

        estado++;
    }

    return 0;
}


static void cargaESTADO()
{
    //pone a cero todos los desplazamientos
    offset_H_PlanoA = 0;     offset_V_PlanoA = 0;
    offset_H_PlanoB = 0;     offset_V_PlanoB = 0;

    if(estado==5) //añade un sprite de Sonic
    {
        Sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    }

    //finalmente activamos el cerrojo para no salirse del bucle secundario
    cerrojo = 1;

}



static void actualizaCamara()
{

    if(estado == 1)
    {
        //PLANO A: scroll horizontal
        VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
    }

    if(estado == 2)
    {
        //PLANO A: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
        VDP_setVerticalScroll(  PLAN_A, offset_V_PlanoA);
    }

    if(estado == 3)
    {
        //PLANO A: no vamos a moverlo pero queremos pintarlo como desde el inicio
        //estas dos lineas hacen falta para que se pinte con un offset de 0 tanto
        //en horizontal como en vertical
        VDP_setHorizontalScroll(PLAN_A, 0);
        VDP_setVerticalScroll(  PLAN_A, 0);
        //PLANO B: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_B, offset_H_PlanoB);
        VDP_setVerticalScroll(  PLAN_B, offset_V_PlanoB);
    }

    if(estado == 4)
    {
        //PLANO A: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
        VDP_setVerticalScroll(  PLAN_A, offset_V_PlanoA);
        //PLANO B: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_B, offset_H_PlanoB);
        VDP_setVerticalScroll(  PLAN_B, offset_V_PlanoB);
    }

    if(estado == 5)
    {
        //PLANO A: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
        VDP_setVerticalScroll(  PLAN_A, offset_V_PlanoA);
        //PLANO B: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_B, offset_H_PlanoB);
        VDP_setVerticalScroll(  PLAN_B, offset_V_PlanoB);
    }

    if(estado == 6)
    {
        //PLANO A: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
        VDP_setVerticalScroll(  PLAN_A, offset_V_PlanoA);
        //PLANO B: scroll horizontal y vertical
        VDP_setHorizontalScroll(PLAN_B, offset_H_PlanoB);
        VDP_setVerticalScroll(  PLAN_B, offset_V_PlanoB);
        //PLANO de SPRITES: actualiza su posicion
        SPR_setPosition(Sonic, posx, posy);
    }

}

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if(estado == 1) //pad: mueve scroll | tecla A: salta al siguiente estado
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_A) cerrojo=0; //abre el cerrojo para salir del bucle secundario
    }

    if(estado == 2) //pad: mueve scroll | tecla B: salta al siguiente estado
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_UP)        { offset_V_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 3) //pad: mueve scroll | tecla C: salta al siguiente estado
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoB -= aceleracionPlanoB; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoB += aceleracionPlanoB; }

        if (value & BUTTON_UP)        { offset_V_PlanoB += aceleracionPlanoB; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoB -= aceleracionPlanoB; }

        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 4) //pad: mueve scroll | tecla A: salta al siguiente estado
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_UP)        { offset_V_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_LEFT)      { offset_H_PlanoB += aceleracionPlanoB; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoB -= aceleracionPlanoB; }

        if (value & BUTTON_UP)        { offset_V_PlanoB += aceleracionPlanoB; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoB -= aceleracionPlanoB; }

        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 5) //pad: mueve scroll y anima el sprite | tecla B: salta al siguiente estado
    {
        if (value & BUTTON_LEFT)
        {
            //plano
            offset_H_PlanoA += aceleracionPlanoA; offset_H_PlanoB += aceleracionPlanoB;
            //sprite
            SPR_setAnim(Sonic, ANIM_RUN);
        }
        if(value & BUTTON_RIGHT)
        {
            offset_H_PlanoA -= aceleracionPlanoA; offset_H_PlanoB -= aceleracionPlanoB;
            SPR_setAnim(Sonic, ANIM_RUN);
        }

        //si no pulsamos, Sonic animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 6) //pad: mueve scroll Y sprite, con limites | tecla C: reiniciar
    {
        if (value & BUTTON_LEFT)
        {
            //plano
            offset_H_PlanoA += aceleracionPlanoA; offset_H_PlanoB += aceleracionPlanoB;
            //sprite (limites para que no se salga de la pantalla)
            posx--; if(posx<=LIMITE_IZQ) posx=LIMITE_IZQ;
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, TRUE);
        }
        if(value & BUTTON_RIGHT)
        {
            posx++; if(posx>=LIMITE_DCH) posx=LIMITE_DCH;
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, FALSE);
            offset_H_PlanoA -= aceleracionPlanoA; offset_H_PlanoB -= aceleracionPlanoB;
        }

        //si no pulsamos, Sonic animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

        //reinicia la ROM
        if (value & BUTTON_C) SYS_hardReset();  //hay que añadir #include "sys.h" en la cabecera
    }

}



static void muestraMENSAJES()
{
    if(estado == 1) //estado inicial al empezar el programa
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: OFF    ", 0, 0);
    VDP_drawText("PLANO B  off                            ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve plano A (solo DCHA e IZQ)   ", 0, 3);
    VDP_drawText("Pulsa -A- para continuar                ", 0, 4);
    }

    if(estado == 2)
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: ON     ", 0, 0);
    VDP_drawText("PLANO B  off                            ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve plano A (todas direcciones) ", 0, 3);
    VDP_drawText("Pulsa -B- para continuar                ", 0, 4);
    }

    if(estado == 3)
    {
    VDP_drawText("PLANO A  off                            ", 0, 0);
    VDP_drawText("PLANO B  scroll H: ON, scroll V: ON     ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve plano B                     ", 0, 3);
    VDP_drawText("Pulsa -C- para continuar                ", 0, 4);
    }

    if(estado == 4)
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: ON     ", 0, 0);
    VDP_drawText("PLANO B  scroll H: ON, scroll V: ON     ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve planos A y B                ", 0, 3);
    VDP_drawText("Pulsa -A- para continuar                ", 0, 4);
    }

    if(estado == 5)
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: OFF    ", 0, 0);
    VDP_drawText("PLANO B  scroll H: ON, scroll V: OFF    ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve planos. El SPRITE esta FIJO!", 0, 3);
    VDP_drawText("Pulsa -B- para continuar                ", 0, 4);
    }

    if(estado == 6)
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: OFF    ", 0, 0);
    VDP_drawText("PLANO B  scroll H: ON, scroll V: OFF    ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("El SPRITE se mueve por la pantalla      ", 0, 3);
    VDP_drawText("Pulsa -C- para Reiniciar la ROM         ", 0, 4);
    }

}
