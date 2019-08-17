/**
 *      @Title:  Leccion 05 - "Sprites (3) enemigos"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#include "fondos.h"   //carga las 2 imágenes de background
#include "sprites.h"  //carga el sprite de Sonic

#define ANIM_STAND      0
#define ANIM_RUN        3

//Declaramos la funcion para recoger la entrada del mando
static void handleInput();

//Funcion para mover enemigos
static void mueveEnemigos();

//Funcion para pintar la pos Y de la avispa
static void pinta_posicion();

//Declaramos una variable (un puntero) para referirnos al sprite de Sonic
Sprite* mi_sonic;

// Posicion en pantalla del sprite
int posx = 64;
int posy = 155;

//Enemigos
Sprite* enemigo[2];

//Posicion en pantalla de los enemigos
u16 enemigoPosx[2];
int enemigoPosy[2];
int enemigoSentidoMovimiento;

//Para el mov senoidal
#define CONST 84


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
    VDP_drawImageEx(PLAN_B, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &fondo2, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo2.tileset->numTile;

    //recoje la paleta de sonic y la mete en la 3a paleta del sistema
    VDP_setPalette(PAL2,sonic_sprite.palette->data);

    //añade el sprite de Sonic
    mi_sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

    //añade los enemigos

        //posicion
        enemigoPosx[0] = 128;
        enemigoPosy[0] = 164;
        enemigoPosx[1] = 260;
        enemigoPosy[1] = 84;
        enemigoSentidoMovimiento =  1;

        //recoge la paleta de los enemigos y la mete en la 4a paleta del sistema
        VDP_setPalette(PAL3,enemies_sprite.palette->data);

        //sprite
        enemigo[0] = SPR_addSprite(&enemies_sprite, enemigoPosx[0], enemigoPosy[0], TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
        enemigo[1] = SPR_addSprite(&enemies_sprite, enemigoPosx[1], enemigoPosy[1], TILE_ATTR(PAL3, TRUE, FALSE, FALSE));

        //asigna la animacion correcta (o todos tendrán la primera animación del fichero png)
        SPR_setAnim(enemigo[0], 1); //asigna la animacion del cangrejo (la segunda, 1, del png)
        SPR_setAnim(enemigo[1], 0); //asigna la animacion de la avispa (la primera, 0, del png) <- no es necesaria

        SPR_update(); // <- no es necesaria pero recomendable


    //Bucle principal
    while(TRUE)
    {
        //recoje la entrada de los mandos
        handleInput();

        //mueve los enemigos
        mueveEnemigos();

        //pinta la pos de la avispa
        pinta_posicion();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }

    return 0;
}

//Funcion handleInput()  recoje la entrada del mando (izq,dcha) y actualiza la posicion
//del sprite mi_sonic, sumando o restando de la variable posx
//además le damos la vuelta al sprite al movernos horizontalmente
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
    {
        SPR_setPosition(mi_sonic, posx--, posy);
        SPR_setHFlip(mi_sonic, TRUE);
        SPR_setAnim(mi_sonic, ANIM_RUN);
        //SPR_setVFlip(mi_sonic, TRUE);
    }

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
    {
        SPR_setPosition(mi_sonic, posx++, posy);
        SPR_setHFlip(mi_sonic, FALSE);
        SPR_setAnim(mi_sonic, ANIM_RUN);
        //SPR_setVFlip(mi_sonic, FALSE);
    }

    //si no pulsamos
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
    {
        SPR_setAnim(mi_sonic, ANIM_STAND);
    }
}

//simplemente al llegar a un extremo el enemigo se da la vuelta
static void mueveEnemigos()
{
    //velocidad de movimiento de los enemigos
    int velocidadmovx = 1;
    int acelerador = 1;     //modificador el mov sinoidal (CAMBIALO PARA PROBAR)
    int mod_amplitud = 1;   //modificador el mov sinoidal (CAMBIALO PARA PROBAR)

    //CANGREJO ( =enemigo[0] )
    //movimiento horizontal simple. Al llegar a los extremos de la pantalla rebota.
    enemigoPosx[0] += velocidadmovx * enemigoSentidoMovimiento;
    if(enemigoPosx[0]>=320 || enemigoPosx[0]<=0) enemigoSentidoMovimiento *= -1;

    //AVISPA ( =enemigo[1] )
    //mov horizontal simple, no hay rebote, aparecerá por el otro lado de la pantalla
    //movimiento vertical senoidal
    enemigoPosx[1] -= velocidadmovx;
    enemigoPosy[1] = CONST + sinFix16(enemigoPosx[1]* acelerador) * mod_amplitud;

    //actualiza la posicion en el VDP
    SPR_setPosition(enemigo[0], enemigoPosx[0], enemigoPosy[0]);
    SPR_setPosition(enemigo[1], enemigoPosx[1], enemigoPosy[1]);

}


//Función pinta_posicion() : Escribe en pantalla la posición del sprite
static void pinta_posicion()
{
    //declaramos una cadena de caracteres
    //sprintf : pasa un valor numerico(posx / posy) a caracteres y los copia
    //en la cadena anterior. %4d alinea a la derecha, importante para al pasar
    //de numeros negativos a positivos todo salga correctamente

    char cadena1[32];
    sprintf(cadena1, "Y(avispa):  %3d", enemigoPosy[1]);
    VDP_drawText(cadena1, 2, 2);

}
