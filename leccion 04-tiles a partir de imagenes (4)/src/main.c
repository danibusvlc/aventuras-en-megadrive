/**
 *      @Title:  Leccion 04 - "Tiles a partir de imágenes (4)"
 *      @Author: Daniel Bustos "danibus"
 */

#include "genesis.h"
#include "resources.h"

//declaracion de funciones
static void handleInput();

int main()
{
    //vector donde copiaremos los colores de la/s paleta/s
    //NO ES LA PALETA, es un vector y modificarlo no modifica la paleta
    u16 paleta_completa[64];

    //para llevar la cuenta de tiles en VRAM
    u16 ind;

    //Tiles en VRAM desde la 2a pos (1er tile para pintar el fondo)
    ind = 1;

    //desactiva el acceso al VDP
	/*
	Desactiva temporalmente cualquier tipo de interrupción (Vertical, Horizontal and External) al
	VDP, de esa forma podemos 'toquetearlo' a placer sin que una interrupción pare lo que estamos
	haciendo y deje a mitad el trabajo
    */
    SYS_disableInts();

    //Inicializa a 320x240px
    VDP_setScreenWidth320();

    //Ponemos TODA LA PALETA (las 4 paletas) completamente en negro
    /* VDP_setPaletteColors( indice_color(0-63), valor_RGB, num_colores_afectados)
	  Esta función pone un total de 'num_colores_afectados' al valor 'valor_RGB'.
	  Esto lo hace desde el color 'indice_color' hasta 'indice_color+num_colores_afectados'
	  En este caso: del color 0 al 63, todos -> un total de 64 colores
	*/
	// El motivo: hacer un fade_in (de pantalla negra a pantalla normal)
    VDP_setPaletteColors(0, (u16*) palette_black, 64); //palette_black = paleta SGDK negros

    //carga las imagenes en VRAM e incrementa ind
    VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 20, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;

    //Volvemos a activar las interrupciones del VDP
    SYS_enableInts();

    //recoge las paletas de cada imagen y mete dichos valores en el vector
    memcpy(&paleta_completa[0],  bgb_image.palette->data, 16 * 2);
    memcpy(&paleta_completa[16], bga_image.palette->data, 16 * 2);

    //FACE IN
    /* VDP_fadeIn(from_color, to_color, pal_final, num_frames, asyn)
       Esta función realiza un fade-in de la paleta actual a la paleta 'pal_final',
       utilizando para ello 'num_frames'. Tener en cuenta 60 frames/seg (NTCS).
       Podemos elegir para qué colores. En este caso, todos:
                     from_color = 0 hasta el color to_color=63
       'asyn' =0 o FALSE  --> El programa se detiene hasta que termine el fade-in
       'asyn' =0 o TRUE   --> El programa continua mientras se hace el fade-in
    */
    //Probar a cambiar TRUE y FALSE
    VDP_fadeIn(0, 63, paleta_completa, 100, FALSE);

    while(1)
    {
        //leer controles, se llama siempre una vez por frame
        handleInput();

        VDP_waitVSync();
    }

    return 0;
}


static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);

    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)    VDP_drawText("IZQUIERDA", 8, 10);

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)   VDP_drawText("DERECHA", 20, 10);

    //si pulsamos izquierda...
    if (value & BUTTON_UP)      VDP_drawText("ARRIBA", 16, 8);

    //si pulsamos derecha...
    if (value & BUTTON_DOWN)    VDP_drawText("ABAJO", 16, 12);

    //si pulsamos A...
    if (value & BUTTON_A)       VDP_drawText("BOTON A", 6, 18);

    //si pulsamos B...
    if (value & BUTTON_B)       VDP_drawText("BOTON B", 14, 18);

    //si pulsamos C...
    if (value & BUTTON_C)       VDP_drawText("BOTON C", 22, 18);

    //si pulsamos X...
    if (value & BUTTON_X)       VDP_drawText("BOTON X", 6, 20);

    //si pulsamos Y...
    if (value & BUTTON_Y)       VDP_drawText("BOTON Y", 14, 20);

    //si pulsamos Z...
    if (value & BUTTON_Z)       VDP_drawText("BOTON Z", 22, 20);

    //si pulsamos START...
    if (value & BUTTON_START)   VDP_drawText("START", 10, 16);

    //si pulsamos MODE...
    if (value & BUTTON_MODE)    VDP_drawText("MODE", 20, 16);

   //si NO pulsamos nada, borra todo del título hacia abajo (VDP_clearTextArea() trabaja en tiles)
    if ((!(value & BUTTON_UP))   && (!(value & BUTTON_DOWN)) &&(!(value & BUTTON_LEFT)) &&
        (!(value & BUTTON_RIGHT))&& (!(value & BUTTON_A))    &&(!(value & BUTTON_B))    &&
        (!(value & BUTTON_C))    && (!(value & BUTTON_X))    &&(!(value & BUTTON_Y))    &&
        (!(value & BUTTON_Z))    && (!(value & BUTTON_MODE)) &&(!(value & BUTTON_START)))
        VDP_clearTextArea(0,7,40,28);

}
