/**
 *      @Title:  "leccion 09 - "scroll (1) simple"
 *      @Author: Daniel Bustos "danibus"
 */


/*
    ESTADO 1: Estado inicial. Mov del plano A en horizontal
    ESTADO 2: Mov del plano A en horizontal y vertical
    ESTADO 3: Mov del plano B en horiz+verti, plano A fijo.
    ESTADO 4: Mov de planos A y B
    ESTADO 5: Mov de planos A y B solo en horiz. Mov Sprite.
    ESTADO 6: Mov de planos A, B y Sprite (limitado)

*/



#include <genesis.h>

#include "gfx.h"     //carga las 2 imágenes de background
#include "sprite.h"  //carga los 3 sprites (anillo,sonic,enemies)

#define SFX_JUMP        64
#define SFX_ROLL        65
#define SFX_STOP        66

#define ANIM_STAND      0
#define ANIM_WAIT       1
#define ANIM_WALK       2
#define ANIM_RUN        3
#define ANIM_BRAKE      4
#define ANIM_UP         5
#define ANIM_CROUNCH    6
#define ANIM_ROLL       7


#define MAX_SPEED       FIX32(8)
#define RUN_SPEED       FIX32(6)
#define BRAKE_SPEED     FIX32(2)

#define JUMP_SPEED      FIX32(-7)
#define GRAVITY         FIX32(0.3)
#define ACCEL           1
#define DE_ACCEL        FIX32(0.15)

#define MIN_POSX        FIX32(10)
#define MAX_POSX        FIX32(400)
#define MAX_POSY        FIX32(156)




//Declaracion de funciones
static void handleInput();
static void cargaESTADO();
static void muestraMENSAJES();
static void actualizaCamara();

//Punteros para referirse a los sprites
Sprite* sprites[3];
Sprite* Sonic;

fix32 camposx;    //para controlar la cámara
fix32 camposy;

s16 xorder;		//será -1 si Joy pul izq, será +1 si Joy pul dcha
s16 yorder;     //será -1 si Joy pul arriba, será +1 si Joy pul abajo
                //si soltamos, ambas serán 0

fix32 enemyPosx[2];
fix32 enemyPosy[2];
s16 enemyXorder[2];







//Para los estados
int estado = 1;     //en este ej comienzo con uno para evitar lios
int cerrojo = 0;

//cuenta de tiles en VRAM
u16 ind;

//desplazamiento respecto al punto (0,0) de los planos
s16 offset_H_PlanoA = 0;
s16 offset_V_PlanoA = 0;
s16 offset_H_PlanoB = 0;
s16 offset_V_PlanoB = 0;

//movimiento de los planos
int aceleracionPlanoA = 2;
int aceleracionPlanoB = 1;

//movimiento de Sonic
int aceleracionSonic = 3;
s16 posx = 48;          //POSICION de Sonic
s16 posy = 158;
s16 movx;               //ACELERACION de Sonic
s16 movy;
#define LIMITE_IZQ  20  //limites pantalla del sprite
#define LIMITE_DCH  260



//DEBUG: Cambiar valor a 1 y ver msgs en GensKmod
int debug = 0;


int main()
{

    //cargamos en memoria planos, sprites, paletas

    //disable interrupt when accessing VDP
    SYS_disableInts();

    //320x224px
    VDP_setScreenWidth320();

    //paletas
    VDP_setPalette(PAL0, bgb_image.palette->data);
    VDP_setPalette(PAL1, bga_image.palette->data);
    VDP_setPalette(PAL2, sonic_sprite.palette->data);
    VDP_setPalette(PAL3, enemies_sprite.palette->data);

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //backgrounds
    ind = TILE_USERINDEX;
    KLog_S1("ind ( 0 )=",ind);          //16
    VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bgb_image.tileset->numTile;
    KLog_S1("ind ( 2 )=",ind);          //378  378-16=362 tiles
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;
    KLog_S1("ind ( 3 )=",ind);          //635  635-378=257 tiles

    //VDP process done, we can re enable interrupts
    SYS_enableInts();

    //bucle ppal
    while(TRUE)
    {
        //carga estado
        cargaESTADO();

        //bucle secundario
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
    offset_H_PlanoA = 0;
    offset_V_PlanoA   = 0;

    //ESTADOS 1,2,3,4: scroll horizontal y vertical, por planos
    //esto se aplica a TODOS los planos
    if(estado==1) //una vez se setea, no hace falta cambiarlo en cada estado
    {
        VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    }

    if(estado==5) //añade un sprite de Sonic
    {
        Sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    }



    //finalmente activamos el cerrojo para no salirse del bucle secundario
    cerrojo = 1;






/*
    if(estado==9999)
    {


        camposx = -1;
        camposy = -1;
        posx = FIX32(48);
        posy = MAX_POSY;
        movx = FIX32(0);
        movy = FIX32(0);
        xorder = 0;
        yorder = 0;

        enemyPosx[0] = FIX32(128);
        enemyPosy[0] = FIX32(164);
        enemyPosx[1] = FIX32(300);
        enemyPosy[1] = FIX32(84);
        enemyXorder[0] = 1;
        enemyXorder[1] = -1;

        // init scrolling
        updateCamera(FIX32(0), FIX32(0));

        //Sonic
        sprites[0] = SPR_addSprite(&sonic_sprite, fix32ToInt(posx - camposx), fix32ToInt(posy - camposy), TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
       //Animaciones
        SPR_setAnim(sprites[1], 1);
        SPR_setAnim(sprites[2], 0);
        //Actualiza VDP
        SPR_update();
    }
*/





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
        VDP_setVerticalScroll(  PLAN_A, offset_V_PlanoA   );
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
        VDP_setVerticalScroll(  PLAN_B, offset_V_PlanoB  );
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




    //only debug
    if(debug)
    {
        KLog_S2("scrollHorizontal=",offset_H_PlanoA," , scrollVertical  =",offset_V_PlanoA);
        KLog_S2("Sonic posx=",posx," , posy  =",posx);
    }

}

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if(estado == 1)
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_A) cerrojo=0; //abre el cerrojo para salir del bucle secundario

        if (value & BUTTON_START)
        {
            &bga_image++;
            VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        }

    }

    if(estado == 2)
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_UP)        { offset_V_PlanoA += aceleracionPlanoA; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoA -= aceleracionPlanoA; }

        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 3)
    {
        if (value & BUTTON_LEFT)      { offset_H_PlanoB -= aceleracionPlanoB; }
        else if (value & BUTTON_RIGHT){ offset_H_PlanoB += aceleracionPlanoB; }

        if (value & BUTTON_UP)        { offset_V_PlanoB += aceleracionPlanoB; }
        else if (value & BUTTON_DOWN) { offset_V_PlanoB -= aceleracionPlanoB; }

        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 4)
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

    if(estado == 5)
    {
        if (value & BUTTON_LEFT)
        {
            //plano
            offset_H_PlanoA += aceleracionPlanoA; offset_H_PlanoB += aceleracionPlanoB;
            //sprite
            SPR_setAnim(Sonic, ANIM_RUN);
        }
        //else
        if(value & BUTTON_RIGHT)
        {
            offset_H_PlanoA -= aceleracionPlanoA; offset_H_PlanoB -= aceleracionPlanoB;
            SPR_setAnim(Sonic, ANIM_RUN);
        }

        //si no pulsamos, Sonic animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 6)
    {
        if (value & BUTTON_LEFT)
        {
            //plano
            offset_H_PlanoA += aceleracionPlanoA; offset_H_PlanoB += aceleracionPlanoB;
            //sprite (limites para que no se salga de la pantalla)
            posx--; if(posx<=LIMITE_IZQ) posx=LIMITE_IZQ;
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, TRUE);
        }
        //else
        if(value & BUTTON_RIGHT)
        {
            posx++; if(posx>=LIMITE_DCH) posx=LIMITE_DCH;
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, FALSE);
            offset_H_PlanoA -= aceleracionPlanoA; offset_H_PlanoB -= aceleracionPlanoB;
        }

        //si no pulsamos, Sonic animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

        if (value & BUTTON_C) cerrojo=0;
    }







    //if (value & BUTTON_LEFT)      { posx -= ACCEL; if (movx <=- MAX_SPEED) movx = -MAX_SPEED; }
    //else if (value & BUTTON_RIGHT){ posx += ACCEL; if (movx >=  MAX_SPEED) movx =  MAX_SPEED; }


    // set camera position
    //updateCamera(posx, posy);

    // set sprites position
    //SPR_setPosition(sprites[0], fix32ToInt(posx), fix32ToInt(posy));

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
    //VDP_drawText("                                       ", 1, 27);
    }

    if(estado == 2)
    {
    VDP_drawText("PLANO A  scroll H: ON, scroll V: ON     ", 0, 0);
    VDP_drawText("PLANO B  off                            ", 0, 1);
    VDP_drawText("PLANO W  off                            ", 0, 2);
    VDP_drawText("DPAD: mueve plano A                     ", 0, 3);
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
    VDP_drawText("Pulsa -C- para continuar                ", 0, 4);
    }

}
