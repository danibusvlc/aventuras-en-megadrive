/**
 *      @Title:  Leccion 03 - "tiles y planos (1)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#define MI_TILE 1

const u32 tile[8]=
{
0x44444421,
0x44442211,
0x44422110,
0x44221100,
0x42211000,
0x42110000,
0x21100000,
0x11000000
};

int main( )
{

    //carga el tile en VRAM
    VDP_loadTileData( (const u32 *)tile, MI_TILE, 1, 0);

    //dibuja el tile en el plano A en pos (1,5) con la paleta por defecto (pal 0)
    VDP_setTileMapXY(PLAN_A, MI_TILE, 1, 5);

    //Dibuja el tile usando TILE_ATTR_FULL()
    //primer parámetro: paleta. PAL2 = paleta de verdes
    //segund parámetro: prioridad. 0 = baja prioridad
    //tercer parámetro: volteo vertical. 1 = vflip
    //cuarto parámetro: volteo horizon. 0 = no hflip
    //quinto parámetro: tile referenciado.
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL2, 0, 0, 0, MI_TILE), 3, 5);

    //Dibuja 2 veces el tile, la primera vez en el plano B con pal 0 (grises)
    //la segunda en el plano A con pal1 (rojos),
    VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 0, 0, 0, MI_TILE), 5, 5);
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, MI_TILE), 5, 5);

    //Dibuja 2 veces el tile, igual que antes pero el tile del plano B con alta prioridad
    VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, 1, 0, 0, MI_TILE), 7, 5);
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, MI_TILE), 7, 5);

    //dibuja la tile volteada horizontalmente, verticalmente y luego ambas a la vez
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, 1, 0, 1, MI_TILE), 3, 8);
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, 1, 1, 0, MI_TILE), 5, 8);
    VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL0, 1, 1, 1, MI_TILE), 7, 8);

        while(1)
        {
        VDP_waitVSync();
        }

    return 0;
}
