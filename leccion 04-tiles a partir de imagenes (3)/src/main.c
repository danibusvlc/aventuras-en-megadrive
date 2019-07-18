/**
 *      @Title:  Leccion 04 - "Tiles a partir de imagenes (3)"
 *      @Author: Daniel Bustos "danibus"
 */

#include "genesis.h"
#include "resources.h"

int main()
{
    //para llevar la cuenta de tiles en VRAM
    u16 ind;

    //recoge las paletas de cada imagen
	VDP_setPalette(PAL0, block.palette->data);
	VDP_setPalette(PAL1, block2.palette->data);

    //Tiles en VRAM desde la 2a pos (1er tile para pintar el fondo)
    ind = 1;

    //carga las imagenes en VRAM e incrementa ind
	VDP_drawImageEx(PLAN_A, &block ,  TILE_ATTR_FULL(PAL0, 0, 0, 0, ind),  3,  3, 1, CPU);
	ind += block.tileset->numTile;
	VDP_drawImageEx(PLAN_B, &block2 , TILE_ATTR_FULL(PAL1, 0, 0, 0, ind), 15, 15, 1, CPU);
	ind += block2.tileset->numTile;

       while(1)
       {
       VDP_waitVSync();
       }

return 0;
}
