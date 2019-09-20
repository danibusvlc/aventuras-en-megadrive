/**
 *      @Title:  Leccion 08 - "shadow & higlight (3) SF2"
 *      @Author: Daniel Bustos "danibus"
 */


#include <genesis.h>
#include "fondos.h"  //carga background
#include "sprite.h"  //carga sprites

#define ANIM_STAND      0
#define ANIM_WALK       1

//entrada del mando
static void handleInput();

//Sprites
Sprite* spr_ryu;
Sprite* spr_sombra;
Sprite* spr_hadoken;

//Posicion inicial en pantalla de los sprites
u32 posx = 120;
u32 posy = 120;
u32 hadoken_posx = 150;
u32 hadoken_posy = 130;

//Prioridad del hadoken
bool prioridad_hadoken = TRUE;

int main()
{
    //variable para llevar el control de tiles
    u16 ind;

    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //recoje las paletas de los fondos y los asigna a la primera y segunda paleta del sistema
    VDP_setPalette(PAL0,fondo1.palette->data);
    VDP_setPalette(PAL1,fondo2.palette->data);

    //carga los fondos en el VDP
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;
    VDP_drawImageEx(PLAN_B, &fondo2, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo2.tileset->numTile;

    //recoje la paleta de los sprites
    VDP_setPalette(PAL2,mi_sprite_ryu.palette->data);
    VDP_setPalette(PAL3,mi_sprite_sombra.palette->data);

    //añade sprites
    spr_ryu = SPR_addSprite(&mi_sprite_ryu, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    spr_sombra = SPR_addSprite(&mi_sprite_sombra, posx, posy+70, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
    spr_hadoken = SPR_addSprite(&mi_sprite_hadoken, hadoken_posx, hadoken_posy, TILE_ATTR(PAL3, prioridad_hadoken, FALSE, FALSE));

    VDP_setHilightShadow(1);    // Highlight/shadow ACTIVO

    SPR_setHFlip(spr_ryu, TRUE);
    SPR_setHFlip(spr_hadoken, TRUE);


	//AYUDA EN PANTALLA
	VDP_setTextPalette(PAL3); //paleta de la sombra para el texto, SGDK coge color15 de dicha paleta
	VDP_drawText("SHADOW & HIGHLIGHT : SF2                ", 2, 1);
    VDP_drawText("D-PAD: MOV RYU, A/B: MOV HA-DO-KEN      ", 2, 2);

    SPR_update();

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

//Recoge la entrada del mando y actualiza la posicion del sprite
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
    {
        SPR_setPosition(spr_ryu, posx--, posy);
        SPR_setPosition(spr_sombra, posx--, posy+70);
        SPR_setAnim(spr_ryu, ANIM_WALK);
        SPR_setHFlip(spr_ryu, FALSE);
    }

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
    {
        SPR_setPosition(spr_ryu, posx++, posy);
        SPR_setPosition(spr_sombra, posx++, posy+70);
        SPR_setAnim(spr_ryu, ANIM_WALK);
        SPR_setHFlip(spr_ryu, TRUE);
    }

    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(spr_ryu, posx, posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(spr_ryu, posx, posy++);

    //si pulsamos A
    if (value & BUTTON_A)
       SPR_setPosition(spr_hadoken, hadoken_posx++, hadoken_posy);
    //si pulsamos B
    if (value & BUTTON_B)
       SPR_setPosition(spr_hadoken, hadoken_posx--, hadoken_posy);



    //si no pulsamos
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
    {
        SPR_setAnim(spr_ryu, ANIM_STAND);
    }

}
