/**
 *      @Title:  Leccion 03 - tiles y planos (3)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

//estas constantes representarán dos tiles, en posiciones de memoria VRAM 1 y 2
#define MI_TILE1 1
#define MI_TILE2 2

/*
  Un tile tiene por definicion 8x8 pixeles:
  -  Cada pixel puede tener un color de una paleta de 16 colores (0..F), esto es 4bpp
  -  4bpp = 4 bits por pixel = 2^4 = 16 colores en total
  Por tanto podemos representar un tile como una matriz de colores de 8x8
  -  Un tile solo puede tener una paleta (pero cualquier paleta)
  -  Un tile se puede dibujar "tal cual", espejado (horizontalmente) y/o invertido (verticalmente)
  -  Un tile NO SE BORRA de un frame a otro, permanece automaticamente gracias al VDP
*/

//creamos 2 vectores donde guardamos 64 valores, cada valor es un color (0..F)
//en cada posición del vector guardamos 8 valores de 8 posiciones
//8x8=64=1 Tile
const u32 tile1[8]=
{
0x00000001,
0x00000012,
0x00000123,
0x00001234,
0x00012344,
0x00123456,
0x01234567,
0x12345678
};

const u32 tile2[8]=
{
0xAAAAAAAA,
0xABBBBBBA,
0xABBBBBBA,
0xACCCCCCA,
0xADDDDDDA,
0xACCCCCCA,
0xABBBBBBA,
0xAAAAAAAA
};

//declaracion funciones
void reset();


/*  Pasos para dibujar un tile:
    - cargar el tile en VRAM
    - cargar o asignarle su paleta
    - dibujar el tile (elegimos plano, posición).
      recomendado utilizar TILE_ATTR_FULL() para asignar paleta, inversion del tile, etc.
*/


int main( )
{

/* Carga Tiles en VRAM
   Para ello cargamos el valor de los vectores en las posiciones de VRAM indicadas
   por las constantes. El tercer valor indica que queremos cargar 1 tile cada vez
   (podriamos usar vectores de 128 valores y cargar 2 tiles de una vez p.e.)
   El cuarto valor indica la forma de transferir los datos a la VRAM, cero indica
   carga de CPU -> VRAM (la "normal"), no usamos por tanto DMA.
*/

//Ayuda en pantalla
//VDP_drawText("texto", x, y);
VDP_drawText("Leccion 3: TILES Y PLANOS", 1, 1);
VDP_drawText("Ejemplo 1: VDP_setTileMapXY()    ", 1, 3);
VDP_drawText("2 tiles cargados en (5,5) y (7,4)", 1, 4);
VDP_drawText("sin paleta asignada, usa la pal0 ", 1, 5);
VDP_drawText("del SGDK, paleta de grises       ", 1, 6);

VDP_loadTileData( (const u32 *)tile1, MI_TILE1, 1, 0);
VDP_loadTileData( (const u32 *)tile2, MI_TILE2, 1, 0);

//ojo, hemos cargado la tiles en VRAM pero NO hemos indicado aún que se pinten

while(1)
{
    //variable para guardar la entrada del mando (ver despues)
    u16 value;

    //Ejemplo 1
    //dibuja los tiles en posiciones de memoria 1 y 2 (costantes MI_TILE1 y MI_TILE2)
    // en el plano A, en posiciones (5,5) y (7,5). NO hemos indicado paleta a usar,
    // así que el SGDK los pinta con la paleta 0.

    //VDP_setTileMapXY(PLANO (A,B,WINDOW), posicion-en-VRAM, x, y) -> (x,y):tiles,no pixels
    VDP_setTileMapXY(PLAN_A, MI_TILE1, 10, 10);
    VDP_setTileMapXY(PLAN_A, MI_TILE2, 12, 10);

    VDP_drawText("Pulsa -A- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }


    //Ejemplo 2
    //Vamos a dibujar por áreas, rellenando cada área con tiles
    //En este caso 2 areas de tiles, cesped y suelo
    VDP_drawText("Ejemplo 2: VDP_fillTileMapRect() ", 1, 3);
    VDP_drawText("rellena un area con un tile determ", 1, 4);
    VDP_drawText("usaremos las paletas pal1 y pal2, ", 1, 5);
    VDP_drawText("del SGDK, rojos y verdes          ", 1, 6);

    //VDP_fillTileMapRect(plano, tile o atributos, x, y, ancho, alto)
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL2, 0, 0, 0, MI_TILE1), 0, 21, 40, 1);
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, MI_TILE2), 0, 22, 40, 7);

    VDP_drawText("Pulsa -B- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }

    VDP_drawText("Hemos usado TILE_ATTR_FULL() para ", 1, 3);
    VDP_drawText("indicar la paleta. En el siguiente", 1, 4);
    VDP_drawText("ejemplo veremos otros usos, como  ", 1, 5);
    VDP_drawText("voltear y espejar tiles.          ", 1, 6);

    VDP_drawText("Pulsa -C- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }

    VDP_drawText("Notese que los tiles dibujados en ", 1, 3);
    VDP_drawText("el ejemplo 1 no se han borrado,   ", 1, 4);
    VDP_drawText("por defecto el VDP mantiene en    ", 1, 5);
    VDP_drawText("pantalla los tiles dibujados.     ", 1, 6);

    VDP_drawText("Pulsa -A- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }




    //Ejemplo 3
    //Vamos a dibujar 1 tile de todas las formas posibles en 4 areas
    //CADA AREA CON UNA PALETA (PARA MEJOR DIFERENCIACION)
    //area 1: tile "tal cual"
    //area 2: tile espejado/volteado horizontalmente
    //area 3: tile espejado/volteado verticalmente
    //area 4: tile espejado/volteado horizontal y verticalmente
    VDP_drawText("Ejemplo 3: Vamos a dibujar 4 AREAS", 1, 3);
    VDP_drawText("area 1 (gris): tile 'tal cual'         ", 1, 4);
    VDP_drawText("area 2 (rojo): volteado verticalmente  ", 1, 5);
    VDP_drawText("area 3 (verd): volteado horizontalmente", 1, 6);
    VDP_drawText("area 4 (azul): tanto horizont como vert", 1, 7);

    //TILE_ATTR_FULL(paleta, prioridad, volteo-vertical, volteo-horizontal, tile)
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL0, 0, 0, 0, MI_TILE1),  0, 10, 20, 9);
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 1, 0, MI_TILE1), 20, 10, 20, 9);
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL2, 0, 0, 1, MI_TILE1),  0, 19, 20, 9);
    VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL3, 0, 1, 1, MI_TILE1), 20, 19, 20, 9);

    VDP_drawText("Pulsa -B- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }



    //Ejemplo 4
    //Bonus: Cambio de color, cambio indice para pintar el fondo
    VDP_drawText("Ejemplo 4: Cambio de color             ", 1, 3);
    VDP_drawText(" Cambio el color del fondo, primero    ", 1, 4);
    VDP_drawText(" asignando otro color de la paleta     ", 1, 5);
    VDP_drawText(" para pintarlo                         ", 1, 6);
    VDP_drawText("                                       ", 1, 7);

    VDP_setBackgroundColor(20);

    VDP_drawText("Pulsa -C- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }

     //Ejemplo 5
    //Bonus: Cambio de color, cambio de color del mismo indice
    VDP_drawText("Ejemplo 5: Cambio de color             ", 1, 3);
    VDP_drawText(" Cambio el color del fondo, ahora en   ", 1, 4);
    VDP_drawText(" cambio voy a cambiar el color sin     ", 1, 5);
    VDP_drawText(" cambiar de indice (estoy cambiando la ", 1, 6);
    VDP_drawText(" paleta a mano)                        ", 1, 7);
    VDP_setPaletteColor(20,RGB24_TO_VDPCOLOR(0x0098e5));

    VDP_drawText("Pulsa -A- para continuar", 1, 8);
    while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }

    //hacemos un reset
    reset();

    VDP_waitVSync();
}

return 0;
}


// soft reset
void reset()
{
    asm("move   #0x2700,%sr\n\t"
        "move.l (0),%a7\n\t"
        "move.l (4),%a0\n\t"
        "jmp    (%a0)");
}
