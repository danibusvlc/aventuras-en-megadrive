/**
 *      @Title:  "leccion 13 - el plano window"
 *      @Author: Daniel Bustos "danibus"
 */


/*
    ESTADO 1: HUD en la parte superior                 // Top HUD.
    ESTADO 2: HUD en parte inferior                    // Down HUD
    ESTADO 3: HUD a la izquierda                       // Left HUD
    ESTADO 4: HUD a la derecha                         // Right HUD
    ESTADO 5: HUD en la parte inferior CON prioridad   // Top HUD priority ON
    ESTADO 6: HUD en la parte inferior SIN prioridad   // Top HUD priority OFF
    ESTADO 7: HUD superior+izquierda                   // Top HUD  + Left HUD
    ESTADO 8: HUD inferior+derecha                     // Down HUD + Right HUD
*/

#include <genesis.h>

#include "gfx.h"     //carga fondos  / load background images
#include "sprite.h"  //carga sprite  / load sprite
#include "sys.h"     //para el reset / to do reset

#define ANIM_STAND      0    //animaciones de Sonic / Sonic animations
#define ANIM_RUN        3

#define LIMITE_LEFT     30   //limites para el sprite / Sonic sprite limits
#define LIMITE_RIGHT    200
#define LIMITE_UP       30
#define LIMITE_DOWN     200


//Declaracion de funciones / Functions
static void handleInput();
static void cargaESTADO();
static void actualizaCamara();

//Puntero a sprites / Sprite pointers
Sprite *Sonic;

//Variables

    //Para ir viendo los distintos ejemplos / to follow all exercise conditions
    int estado;
    int cerrojo = 0;

    //Puntos / Score
    s16 P1score = 100;
    s16 P2score = 130;
    s16 highscore = 10000;
    char txt1[8], txt2[8], txt3[8];

    //Movimiento de los planos respecto al punto (0,0) / Plans movement regarding point (0,0)
    s16 offset_H_PlanoA;
    s16 offset_H_PlanoB;

    //Movimiento de los planos / plan movement
    int aceleracionPlanoA = 2;
    int aceleracionPlanoB = 1;

    //posicion y movimiento de Sonic / Sonic position and movement
    int aceleracionSonic = 3;
    s16 posx;
    s16 posy;

    //Tileset del HUD / HUD tileset
    Map *MyTilesetH, *MyTilesetV;

    //nuestra cuenta de tiles en VRAM / our VRAM own index
    //el tilesetH comienza en ind1, el tilesetV comienza en ind2
    u16 ind, ind2, ind3;



int main()
{
    //valores iniciales / initial values
    estado = 1;
    offset_H_PlanoA = 0;
    offset_H_PlanoB = 0;
    posx = 108;
    posy = 158;

    //320x224px
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();

    //paletas / palettes
    VDP_setPalette(PAL0, bgb_image.palette->data);
    VDP_setPalette(PAL1, bga_image.palette->data);
    VDP_setPalette(PAL2, sonic_sprite.palette->data);
    VDP_setPalette(PAL3, horizontal_hud.palette->data); //misma paleta para el vertical hud / same palette for vertical hud

    //inicializa motor de sprites / Start sprite engine
    SPR_init(0, 0, 0);

    //El tile 0 está reservado para uso del SGDK / Tile 0 is reserved by SGDK
    ind = 1;

    //Fondos / background images
    VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;

    //Tileset HUD arriba/abajo          /   top/dowm
    MyTilesetH = unpackMap(horizontal_hud.map, NULL);
    VDP_loadTileSet(horizontal_hud.tileset, ind, CPU);
    ind2 = ind + horizontal_hud.tileset->numTile;

    //Tileset HUD izquierda/derecha     /   left/right
    MyTilesetV = unpackMap(vertical_hud.map, NULL);
    VDP_loadTileSet(vertical_hud.tileset, ind2, CPU);
    ind3 = ind2 + horizontal_hud.tileset->numTile;

    //Sonic Sprite
    Sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

    //Scroll (por planos) / Scroll (by planes)
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    //bucle ppal / main loop
    while(TRUE)
    {
        //Inicializar cada ejemplo / Initialize each example
        //Además cierra el cerrojo del bucle secundario / Close secondary loop lock
        cargaESTADO();

        //bucle secundario: se ejecuta mientras cerrojo=1, se abre (cerrojo=0) para cambiar ejemplo=estado.
        //secondary loop: working while lock is (cerrojo=1), it's openend (cerrojo=0) to change example=condition.
        while(cerrojo)
        {
            //Controles (sincronos) / Sincr. controls
            handleInput();

            //actualiza Camara / Update Camera
            actualizaCamara();

            //actualiza VDP / VDP update
            SPR_update();

            //Espera al barrido vertical TV / vertical TV sync
            VDP_waitVSync();
        }

        estado++;
    }

    return 0;
}

//Esta funcion configura todo correctamente para cada ejemplo. Resetea el scroll.
//This function configure properly each example. Make reset to scroll.
static void cargaESTADO()
{
    //pone a cero todos los desplazamientos
    offset_H_PlanoA = 0;
    offset_H_PlanoB = 0;

    if(estado == 1){  //HUD superior

        VDP_setWindowVPos(FALSE, 0);    //elimina el plano W anterior

        VDP_setHilightShadow(0);  // Desactiva Highlight/shadow / Disable Highlight/shadow

        VDP_setWindowVPos(FALSE, 2);    //plano W: 2 filas (filas 0 y 1) / W plan: 2 rows (rows 0 and 1)

        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset horizontal)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, ind), 0, 0, 0, 0, 40, 2);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 1);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 1);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 1);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"EJEMPLO HUD SUPERIOR ", 10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'A' sig EJEMPLO", 10,10);
    }

    if(estado == 2){  //HUD INFERIOR

        VDP_setWindowVPos(FALSE, 0);    //elimina el plano W anterior
        VDP_setWindowVPos(TRUE, 26);    //plano W desde la fila 25 hasta el final (25,26,27)
        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset horizontal)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind), 0, 26, 0, 0, 40, 2);
        //pinta las puntuaciones
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 27);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 27);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 27);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"EJEMPLO HUD INFERIOR ",  10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'B' sig EJEMPLO",  10,10);
    }

    if(estado == 3){  //HUD IZQUIERDO

        VDP_setWindowVPos(FALSE, 0); //elimina el plano W anterior
        VDP_setWindowHPos(FALSE, 2); //plano W desde la columna 0 hasta la columna 2 (columnas 0, 1 y 2).
        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset vertical)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetV, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind2), 0, 0, 0, 0, 4, 28);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  0,  4);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2,  0, 13);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3,  0, 23);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"EJEMPLO HUD IZQUIERDA",  10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'C' sig EJEMPLO",  10,10);
    }

    if(estado == 4){  //HUD DERECHO

        VDP_setWindowVPos(FALSE, 0); //elimina el plano W anterior
        VDP_setWindowHPos(TRUE, 18); //plano W desde la columna 36 (18x2) hasta el final
        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset vertical)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetV, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind2), 36, 0, 0, 0, 4, 28);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  37,  4);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2,  37, 13);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3,  37, 23);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"EJEMPLO HUD DERECHA  ",  10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'A' sig EJEMPLO",  10,10);
    }


    if(estado==5) //HUD SUPERIOR CON PRIORIDAD
    {
        VDP_setWindowHPos(FALSE, 0); //elimina el plano W anterior
        VDP_setWindowVPos(FALSE, 2);    //plano W desde 0 a la fila '2' (3 filas: 0, 1 y 2)
        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset horizontal)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, ind), 0, 0, 0, 0, 40, 2);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 1);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 1);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 1);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"TOP HUD PRIORITY = ON    ",10,9);
        VDP_drawTextBG(PLAN_B,"Shadow/Hightlight working",10,10);
        VDP_drawTextBG(PLAN_B,"PULSA 'B' sig EJEMPLO    ",10,10);

        VDP_setHilightShadow(1);    // Activa Highlight/shadow / Enable Highlight/shadow
    }

    if(estado==6) //HUD SUPERIOR SIN PRIORIDAD
    {
        VDP_setWindowVPos(FALSE, 0);    //elimina el plano W anterior
        VDP_setWindowVPos(FALSE, 2);    //plano W desde 0 a la fila '2' (3 filas: 0, 1 y 2)
        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta el HUD (utiliza el tileset horizontal)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind), 0, 0, 0, 0, 40, 2);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 1);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 1);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 1);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"TOP HUD PRIORITY = OFF   ",10,9);
        VDP_drawTextBG(PLAN_B,"Shadow/Hightlight working",10,10);
        VDP_drawTextBG(PLAN_B,"PULSA 'C' sig EJEMPLO    ",10,10);
    }

    if(estado == 7){  //HUD superior+izquierda

        VDP_setWindowVPos(FALSE, 0);    //elimina el plano W anterior

        VDP_setHilightShadow(0);  // Desactiva Highlight/shadow / Disable Highlight/shadow

        VDP_setWindowVPos(FALSE, 2); //plano W desde 0 a la fila '2' (3 filas: 0, 1 y 2)
        VDP_setWindowHPos(FALSE, 2); //plano W desde la columna 0 hasta la columna 2 (columnas 0, 1 y 2).

        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta ambos HUD (utiliza ambos tilesets)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, ind), 0, 0, 0, 0, 40, 2);
        VDP_setMapEx(PLAN_WINDOW, MyTilesetV, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind2), 0, 0, 0, 0, 4, 28);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 1);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 1);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 1);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  0,  4);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2,  0, 13);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3,  0, 23);
        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"HUD SUPERIOR+IZQUIERDA", 10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'A' sig EJEMPLO",  10,10);
    }

    if(estado == 8){  //HUD inferior+derecha

        VDP_setWindowVPos(FALSE, 0);    //elimina el plano W anterior
        VDP_setWindowHPos(FALSE, 0);    //elimina el plano W anterior

        VDP_setWindowVPos(TRUE, 26);    //plano W desde la fila 25 hasta el final.
        VDP_setWindowHPos(TRUE, 18);    //plano W desde la columna 36 hasta el final (18x2)

        VDP_clearTextArea(10,9,22,3);   //borra textos anteriores / delete text from other examples

        //pinta ambos HUD (utiliza ambos tilesets)
        VDP_setMapEx(PLAN_WINDOW, MyTilesetH, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind), 0, 26, 0, 0, 40, 2);
        VDP_setMapEx(PLAN_WINDOW, MyTilesetV, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind2), 36, 0, 0, 0, 4, 28);
        //pinta las puntuaciones en el plano W
        VDP_setTextPlan(PLAN_WINDOW);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1, 37,  4);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 37, 13);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 37, 23);
        intToStr(P1score,   txt1, 0); VDP_drawText(txt1,  4, 27);
        intToStr(highscore, txt2, 0); VDP_drawText(txt2, 20, 27);
        intToStr(P2score,   txt3, 0); VDP_drawText(txt3, 33, 27);

        //ahora pinta en los planos
        VDP_drawTextBG(PLAN_A,"HUD INFERIOR+DERECHA  ", 10,9);
        VDP_drawTextBG(PLAN_B,"PULSA 'B' RESETEAR    ", 10,10);
    }


    //finalmente activamos el cerrojo para no salirse del bucle secundario
    cerrojo = 1;
}


//Actualiza el scroll
static void actualizaCamara()
{
    //en todos los estados
    VDP_setHorizontalScroll(PLAN_A, offset_H_PlanoA);
    VDP_setHorizontalScroll(PLAN_B, offset_H_PlanoB);
}

//Movimiento del Sprite y del scroll    /   Sprite and Scroll movements
static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    // Movimiento de Sonic+Scroll (comun para todos los estados=ejemplos)
    // Sonic and Scroll movement (common for all conditions=examples)
   if (value & BUTTON_LEFT)
    {
        //plans
        offset_H_PlanoA += aceleracionPlanoA;
        offset_H_PlanoB += aceleracionPlanoB;
        //sprite (limites para que no se salga de la pantalla) / (avoid sprite to escape from visible zone)
        posx--; if(posx<=LIMITE_LEFT) posx=LIMITE_LEFT;
        SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, TRUE);
    }
    if(value & BUTTON_RIGHT)
    {
        posx++; if(posx>=LIMITE_RIGHT) posx=LIMITE_RIGHT;
        SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, FALSE);
        offset_H_PlanoA -= aceleracionPlanoA;
        offset_H_PlanoB -= aceleracionPlanoB;
    }
    if (value & BUTTON_UP)  posy--; //mueve a Sonic arriba y abajo
    if (value & BUTTON_DOWN)posy++; //moving Sonic up and down

    SPR_setPosition(Sonic, posx, posy);

    //si no pulsamos, Sonic animacion idle / If press nothing, idle animation for Sonic
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

    if(estado == 1) //HUD superior, TOP HUD / A: salta al sig estado, change to next example
    {
        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 2) //HUD inferior, DOWN HUD / B: salta al sig estado, change to next example
    {
        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 3) //HUD izquierdo, LEFT HUD / C: salta al sig estado, change to next example
    {
        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 4) //HUD derecho, RIGHT HUD / A: SOFT reset
    {
        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 5) //Shadow/Highlight HUD superior

    {
        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 6) //Shadow/Highlight HUD superior (II)
    {
        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 7) //HUD superior+izquierdo    / top HUD + left HUD
    {
        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 8) //HUD inferior+derecho      / down HUD + right HUD
    {
        if (value & BUTTON_B) SYS_reset();
    }

    //reinicia la ROM en cualquier momento / (SOFT) reset ROM in any time
    if (value & BUTTON_START) SYS_reset();

}
