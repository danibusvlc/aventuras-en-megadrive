/**
 *      @Title:  Leccion 05 - "leccion 08-shadow & higlight (4) SF2 Mapas"
 *      @Original_by: RealBrucest
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#include "fondos.h" //carga fondos
#include "sprite.h" //carga sprites

//animaciones
#define ANIM_STAND      0
#define ANIM_WALK       1

// CONSTANTES PARA EL MAPA DE PRIORIDADES/TILES ////////////

//La pantalla (un plano) son 40×28 tiles
#define SCREEN_WIDTH_TILES     40
#define SCREEEN_HEIGHT_TILES   28

#define SCENARIO_POS_X          0
#define SCENARIO_POS_Y          0

#define SCENARIO_WIDTH_TILES    SCREEN_WIDTH_TILES
#define SCENARIO_HEIGHT_TILES   SCREEEN_HEIGHT_TILES
//Total tiles de la pantalla = 40x28 = 1120 tiles
#define SCENARIO_NUM_TILES      SCENARIO_WIDTH_TILES * SCENARIO_HEIGHT_TILES


//declaracion de funciones
static void handleInput();

//variables globales

//sprite
Sprite* spr_ryu;
Sprite* spr_sombra;
Sprite* spr_hadoken;

//Posición inicial en pantalla del sprite
u32 ryu_posx = 120;
u32 ryu_posy = 120;
u32 hadoken_posx = 150;
u32 hadoken_posy = 130;


///////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////
int main()
{
    // DATOS _________________________________________

    // Scenario (planes)
    //crea una vector con tantas posiciones como tiles tiene la pantalla (1120)
    u16 tilemap_buffer[SCENARIO_NUM_TILES];
    //puntero a la primera posición del vector
    u16 *aplan_tilemap = &tilemap_buffer[0];
    //inicializa el vector todo a cero (por defecto hay 'basura')
    for(int j=0; j<SCENARIO_NUM_TILES;j++) tilemap_buffer[j]=0;

    //puntero a la imagen "mapa de prioridades"
    u16 *shadowmap_tilemap = bg_prioridad.map->tilemap;
    //para contar, valor inicial tiles totales de una pantalla
    u16 numtiles = SCENARIO_NUM_TILES;

    // PROCESO ____________________________________________

    // RECORRE tile a tile y según criterio marcamos prioridad (o no)
    // recorremos desde el ultimo al primer tile, para cada uno comprobamos el valor de ese tile
    // en el mapa de prioridades, si es 0 (color negro, indice 0 de paleta), no hacemos nada
    // si NO es cero (cualquier otro color, indice distinto 0 de paleta) marcamos ese tile
    // con prioridad (TILE_ATTR_PRIORITY_MASK)
    while(numtiles--)
    {
        if(*shadowmap_tilemap)
            *aplan_tilemap |= TILE_ATTR_PRIORITY_MASK; //equivale a *aplan_tilemap = *aplan_tilemap | TILE_ATTR_PRIORITY_MASK;
                                                       //TILE_ATTR_PRIORITY_MASK se carga toda la info del tile EXCEPTO la info de prioridad

        aplan_tilemap++;        //avanza en el vector
        shadowmap_tilemap++;    //avanza en el otro vector
    }

    //EN ESTE PUNTO tenemos el vector del mapa de prioridades, en cada posicion tiles configuradas con la prioridad

    // Carga en el plano A en posiciones 0,0 con dimensiones 40x28 tiles el contenido que hay en &tilemap_buffer[0]
    VDP_setTileMapDataRectEx(PLAN_A, &tilemap_buffer[0], 0/*u16 basetile*/,
        SCENARIO_POS_X, SCENARIO_POS_Y,
        SCENARIO_WIDTH_TILES, SCENARIO_HEIGHT_TILES, SCENARIO_WIDTH_TILES);

    // Carga el planoB (el escenario "real" tal cual es, nada raro)
    VDP_drawImage(PLAN_B, &bg_B_real, SCENARIO_POS_X, SCENARIO_POS_Y);

    // Init and load the sprite
    SPR_init(16, 256, 256);

    // Set the palette taken from the scenario (BLAN image)
    VDP_setPalette(PAL0, (u16*) bg_B_real.palette->data);

    // Set the palette from the sprite
    VDP_setPalette(PAL1, (u16*) mi_sprite_ryu.palette->data);
    VDP_setPalette(PAL3, (u16*) mi_sprite_hadoken.palette->data);

    spr_ryu = SPR_addSprite(&mi_sprite_ryu, ryu_posx, ryu_posy, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
    spr_sombra = SPR_addSprite(&mi_sprite_sombra, ryu_posx, ryu_posy+70, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    spr_hadoken = SPR_addSprite(&mi_sprite_hadoken, hadoken_posx, hadoken_posy, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));



    // Enable HL/S mode
    VDP_setHilightShadow(1);

    SPR_setHFlip(spr_ryu, TRUE);
    SPR_setHFlip(spr_hadoken, TRUE);
    SPR_update();

   // LOOP
   while(TRUE)
    {
        handleInput();

        SPR_update();

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
        SPR_setPosition(spr_ryu, ryu_posx--, ryu_posy);
        SPR_setPosition(spr_sombra, ryu_posx--, ryu_posy+70);
        SPR_setAnim(spr_ryu, ANIM_WALK);
        SPR_setHFlip(spr_ryu, FALSE);
    }

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
    {
        SPR_setPosition(spr_ryu, ryu_posx++, ryu_posy);
        SPR_setPosition(spr_sombra, ryu_posx++, ryu_posy+70);
        SPR_setAnim(spr_ryu, ANIM_WALK);
        SPR_setHFlip(spr_ryu, TRUE);
    }

    //si pulsamos arriba...
    if (value & BUTTON_UP)
    {
        SPR_setPosition(spr_ryu, ryu_posx, ryu_posy--);
        SPR_setPosition(spr_sombra, ryu_posx, ryu_posy+69);
    }

    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
    {
        SPR_setPosition(spr_ryu, ryu_posx, ryu_posy++);
        SPR_setPosition(spr_sombra, ryu_posx, ryu_posy+69);
    }

    //si pulsamos A
    if (value & BUTTON_A)
       SPR_setPosition(spr_hadoken, hadoken_posx++, hadoken_posy);
    //si pulsamos A
    if (value & BUTTON_B)
       SPR_setPosition(spr_hadoken, hadoken_posx--, hadoken_posy);

    //si no pulsamos
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
    {
        SPR_setAnim(spr_ryu, ANIM_STAND);
    }

}
