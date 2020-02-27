/**
 *      @Title:  leccion 09 - "scroll (3) mas alla de los 512px"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#include "gfx.h"
#include "sprite.h"

//animacion de Sonic
#define ANIM_STAND      0
#define ANIM_RUN        3

//definicion de funciones
static void handleInput();
static void updateCamera();
static void updatePhysic();
static void show_debug();


//definicion de variables

    //pulsacion DPAD
    s16 Xorder;                 //pulsacion derecha
    s16 Yorder;                 //pulsacion izquierda

    //scroll
    s16 cuentaPixels = 0;       //detecta cambios de tile, se resetea cada 8px (de 0 a 7)
    s16 offset = 0;             //scroll en pixels, de 0 a 1023 px
    s16 column_to_update = 0;   //scroll en TILES,  de 0 a 127 tiles

    //control de tiles del fondo
    u16 ind;

    //sprites
    Sprite *sonic_spr;          //sonic sprite
    s16 SonicX;
    s16 SonicY;




//FUNCION PRINCIPAL
int main()
{
    //disable interrup
    SYS_disableInts();

    //320x224px
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

    //motor sprites
    SPR_init();

    //background
    VDP_setPalette(PAL0, bg_image.palette->data);
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_B, &bg_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bg_image.tileset->numTile;

    //sprite
    VDP_setPalette(PAL1, sonic_sprite.palette->data);
    SonicX = SonicY = 160;
    sonic_spr= SPR_addSprite(&sonic_sprite, SonicX, SonicY, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    // VDP process done, we can re enable interrupts
    SYS_enableInts();

    //Scroll por planos
    VDP_setScrollingMode(HSCROLL_PLANE  ,VSCROLL_PLANE);

    while(TRUE)
    {
        handleInput();
        updatePhysic();
        show_debug();
        SPR_update();

        VDP_waitVSync();
    }

    return 0;
}


//Muestra variables en pantalla
static void show_debug()
{
    char integer_string[32];
    VDP_setTextPalette(PAL0);
    sprintf(integer_string, "%4d", offset);
    VDP_drawText("offset          :", 12, 12);
    VDP_drawText(integer_string,      30, 12);
    sprintf(integer_string, "%4d", cuentaPixels);
    VDP_drawText("cuentaPixels    :", 12, 14);
    VDP_drawText(integer_string,      30, 14);
    sprintf(integer_string, "%4d", column_to_update);
    VDP_drawText("column_to_update:", 12, 16);
    VDP_drawText(integer_string,      30, 16);
    }


//control del scroll
static void updatePhysic()
{
    if (Xorder > 0)         //PULSO DERECHA
    {
        offset++; cuentaPixels++;  if(cuentaPixels>7) cuentaPixels=0;  //del 0 al 7 son 8px=1 TILE
        SPR_setAnim(sonic_spr, ANIM_RUN);
        SPR_setHFlip(sonic_spr, FALSE);
    }
    else if (Xorder < 0)    //PULSO IZQUIERDA
    {
        offset--; cuentaPixels--;  if(cuentaPixels<-7) cuentaPixels=0;
        SPR_setAnim(sonic_spr, ANIM_RUN);
        SPR_setHFlip(sonic_spr, TRUE);
    }
    else SPR_setAnim(sonic_spr, ANIM_STAND);

    updateCamera();
}



static void updateCamera()
{
    //cada vez que nos 'salimos' de la imagen, volvemos al principio
    if(offset>1023) offset=0;
    //if(offset<0) offset=184;

    //solo actualizamos el tile que va a mostrarse a continuación,
    //justo ANTES de mostrarse (de zona no visible a zona visible)
    if(cuentaPixels==0)
    {
        column_to_update = (((offset + 320) >> 3)+ 23) & 127;
                        // 320=screen_width
                        // valor_px >> 3 = valor_tiles ===> de Pixels a Tiles,">>3" equivale a dividir entre 8
                        // valor_tiles + 23 = nuevo_valor_tiles ===> para apuntar a la tile correcta a actualizar
                        // nuevo_valor_tiles & 127              ===> se asegura de devolver un numero de 0 a 127
        VDP_setMapEx(PLAN_B, bg_image.map, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX),
                     column_to_update, 0, column_to_update, 0, 1, 28);
    }
    if(cuentaPixels==-1)
    {
        column_to_update = (((offset + 320) >> 3)+ 88) & 127;
                        // valor_tiles + 88 = nuevo_valor_tiles ===> para apuntar a la tile correcta a actualizar
        VDP_setMapEx(PLAN_B, bg_image.map, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX),
                     column_to_update, 0, column_to_update, 0, 1, 28);
    }

    //hacemos scroll
    VDP_setHorizontalScroll(PLAN_B, -offset);

}


static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_LEFT) Xorder = -1;
    else if (value & BUTTON_RIGHT) Xorder = +1;
        else Xorder = 0;
}

