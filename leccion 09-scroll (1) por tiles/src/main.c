/**
 *      @Title:  "leccion 09 - "scroll (1) por tiles"
 *      @Author: Daniel Bustos "danibus"
 */

/*
    ESTADO 1: Estado inicial. Mov del plano A en horizontal
    ESTADO 2: Mov del plano A en horizontal y vertical
    ESTADO 3: Mov del plano B en horiz+verti, plano A fijo.
    ESTADO 4: Mov de planos A y B en todas direcciones.
    ESTADO 5: Mov de planos A y B solo en horiz. Sprite fijo.
    ESTADO 6: Mov de planos A, B y Sprite (limitado a zona visible).
    ESTADO 7: SHOOT 'EM UP
*/

#include <genesis.h>

#include "gfx.h"     //carga las imágenes de background
#include "sprite.h"  //carga los sprites
#include "sys.h"     //para hacer el reset

#define ANIM_STAND   0   //animaciones de Sonic
#define ANIM_RUN     3

#define ANIM_NAVE_IDLE 0 //animaciones Nave
#define ANIM_NAVE_MOV  1


#define LIMITE_IZQ  10   //limites pantalla para el sprite
#define LIMITE_DCH  280
#define LIMITE_UP   10
#define LIMITE_DOWN 180


//Declaracion de funciones
static void handleInput();
static void cargaESTADO();
static void muestraMENSAJES();
static void actualizaCamara();

//Punteros a sprites
Sprite *Sonic, *Nave, *Enemigos[6];

//Para ir pasando por los distintos ejemplos
int estado  = 1;
int cerrojo = 0;

//Cuenta de tiles en VRAM
u16 ind;

//Desplazamiento respecto al punto (0,0) de los planos (ejemplo 6)
s16 offsetA = 0;
//Para (ir incrementando) el movimiento de los planos (ejemplo 6)
s16 aceleracionA = 4;

//movimiento de Sonic (ejemplo 6)
int posx = 48;
int posy = 158;

//movimiento de las Naves (ejemplo 7)
//re-usaremos 'posx' y 'posy' para la posicion de la nave del player
int enemigo_posx[6] ={0,0,0,0,0,0};
int enemigo_posy[6] ={0,0,0,0,0,0};

//vectores para los ejemplos de scroll (ejemplos 1 al 7)
s16 vector01[6] = {1,2,3,4,5,6};
s16 vector02[6] = {0,10,20,30,40,50};
s16 vector03[10] ={0,0,0,0,0,0,0,0,0,0};
s16 vector04[28] ={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0};
s16 vector05[28] ={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0};
s16 vector06[28] ={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0};

s16 v_aceleracion01[6] = {1,2,3,3,2,1};
s16 v_aceleracion02[10]= {1,2,3,4,5,6,7,8,9,10};
s16 v_aceleracion03[28] ={9,9,8,8, 7,6,6,6, 5,5,4,3, 2,1,2,3, 4,5,5,6, 6,6,7,7, 8,8,9,9};

fix16 vector_aux[28] ={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,};
fix16 aceleracion04 = FIX16(0.06);


int main()
{
    //320x224px
    VDP_setScreenWidth320();

    //paletas
    VDP_setPalette(PAL0, bgb_image.palette->data);
    VDP_setPalette(PAL1, bga_image.palette->data);
    VDP_setPalette(PAL2, sonic_sprite.palette->data);

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //backgrounds
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bgb_image.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bga_image.tileset->numTile;

    //Configura el scroll (por TILES)
    VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);

    //bucle ppal
    while(TRUE)
    {
        //En cada ejemplo llamamos a cargaESTADO() para inicializar cada ejemplo correctamente
        cargaESTADO();

        //bucle secundario, se abre (cerrojo=0) cada vez que cambiamos de ejemplo=estado
        //al iniciarse un ejemplo, se cierra (cerrojo=1)
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
    offsetA = 0;

    //pone algunos vectores a sus valores iniciales (cambian durante los ejemplos)
    for(int i = 0; i < 6;  i++) vector01[i] = i+1;      //del 1 a 6
    for(int i = 0; i < 6;  i++) vector02[i] = i*10;     //del 0 al 50
    for(int i = 0; i < 6;  i++) vector03[i] = 0;        //0
    for(int i = 0; i < 28; i++) vector04[i] = 0;        //0
    for(int i = 0; i < 28; i++) vector05[i] = FIX16(0); //0


    if(estado==6) //añade un sprite de Sonic
    {
        Sonic = SPR_addSprite(&sonic_sprite, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    }

    if(estado == 7)
    {
        //resetea el VDP: fondos, vram, etc
        VDP_resetScreen();
        VDP_init();

        //320x224px
        VDP_setScreenWidth320();

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //paletas
        VDP_setPalette(PAL0, bgd_image.palette->data);
        VDP_setPalette(PAL0, bgc_image.palette->data);
        VDP_setPalette(PAL0, nave_sprite.palette->data);

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //backgrounds
        ind = TILE_USERINDEX;
        VDP_drawImageEx(PLAN_B, &bgd_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bgd_image.tileset->numTile;
        VDP_drawImageEx(PLAN_A, &bgc_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bgc_image.tileset->numTile;

        //Configura el scroll (por TILES)
        VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);

        //posiciones iniciales de los sprites
        posx = 50; posy = 120;
        for(int i = 0; i<6; i++){
            enemigo_posx[i] = 100 + i*50; enemigo_posy[i] = 30 + i*20;
        }

        //crea los sprites
        Nave = SPR_addSprite(&nave_sprite, posx, posy, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

        for(int i = 0; i<6; i++)
            Enemigos[i] = SPR_addSprite(&enemigo_sprite, enemigo_posx[i], enemigo_posy[i], TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    }


    //finalmente activamos el cerrojo para no salirse del bucle secundario
    cerrojo = 1;

}



static void actualizaCamara()
{

    if(estado == 1) //planoA:Estático, planoB:Scroll 6 primeras filas
    {
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector01, 6, CPU);
        for(int i = 0; i < 6; i++) vector01[i] = vector01[i]+1;
    }

    if(estado == 2) //Idem pero con distinto vector inicial
    {
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector02, 6, CPU);
        for(int i = 0; i < 6; i++) vector02[i] = vector02[i]+1;
    }

    if(estado == 3) //Idem pero con distinta velocidad por fila
    {
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector02, 6, CPU);
        for(int i = 0; i < 6; i++) vector02[i] = vector02[i]+i;
    }

    if(estado == 4) //Idem usando vector aceleracion ( v_aceleracion01 )
    {
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector02, 6, CPU);
        for(int i = 0; i < 6; i++) vector02[i] += v_aceleracion01[i];
    }

    if(estado == 5) //Scroll por tiles en 2 zonas diferentes de la TV
    {
        //nubes
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector01, 6, CPU);
        for(int i = 0; i < 6; i++)  vector01[i] += 2;
        //mar
        VDP_setHorizontalScrollTile(PLAN_B, 18, vector03, 10, CPU);
        for(int i = 0; i < 10; i++) vector03[i] -= v_aceleracion02[i];
    }

    if(estado == 6) //scroll por tiles en 2 zonas en plano B y 1 zona en plano A
    {
        //nubes
        VDP_setHorizontalScrollTile(PLAN_B, 0, vector01, 6, CPU);
        for(int i = 0; i < 6; i++)  vector01[i] -= 2;
        //mar
        VDP_setHorizontalScrollTile(PLAN_B, 18, vector03, 10, CPU);
        for(int i = 0; i < 10; i++) vector03[i] -= v_aceleracion02[i];
        //plano A
        VDP_setHorizontalScrollTile(PLAN_A, 0,  vector04, 28, CPU);
        for(int i = 0; i < 28; i++) vector04[i] = offsetA;
    }

    if(estado == 7) //scroll por tiles todo plano B y muchas zonas en plano A
    {
        //plano B: scroll hasta que el planeta llega a mitad de pantalla aprox
        if( vector05[0]>=(-200) )
        {
            VDP_setHorizontalScrollTile(PLAN_B, 0, vector05, 28, CPU);
            for(int i = 0; i < 28; i++){
                vector_aux[i] = fix16Sub(vector_aux[i],aceleracion04);
                vector05[i] = fix16ToInt(vector_aux[i]);
            }
        }

        //plano A
        VDP_setHorizontalScrollTile(PLAN_A, 0, vector06, 28, CPU);
        for(int i = 0; i < 28; i++) vector06[i] -= v_aceleracion03[i];

        //metemos aqui el mov de los enemigos (deberia ir en otro lado, pero por simplificar)
        for(int i = 0; i < 6; i++){
            SPR_setPosition(Enemigos[i], enemigo_posx[i]-=3, enemigo_posy[i]);
            //si se sale por la izq, lo ponemos a la derecha en otra posicion
            if(enemigo_posx[i]<-50){ enemigo_posx[i]=350; enemigo_posy[i]=(((random()%200)-1)+1); } //200 es el max de la coordenada y
        }




    }

}

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if(estado == 1) //abre el cerrojo para salir del bucle secundario
    {
        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 2)
    {
        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 3)
    {
        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 4)
    {

        if (value & BUTTON_A) cerrojo=0;
    }

    if(estado == 5)
    {
        if (value & BUTTON_B) cerrojo=0;
    }

    if(estado == 6) //PAD: Mueve scroll Y sprite
    {
        if (value & BUTTON_LEFT)
        {   //plano
            offsetA += aceleracionA;
            //sprite
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, TRUE);
        }
        if(value & BUTTON_RIGHT)
        {
            offsetA -= aceleracionA;
            SPR_setAnim(Sonic, ANIM_RUN); SPR_setHFlip(Sonic, FALSE);
        }

        //si no pulsamos, Sonic animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT))) SPR_setAnim(Sonic, ANIM_STAND);

        //abre el cerrojo para salir del bucle secundario
        if (value & BUTTON_C) cerrojo=0;
    }

    if(estado == 7) //pad: mueve nave, con limites | tecla START: reiniciar
    {
        if (value & BUTTON_LEFT)
        {
            //sprite (limites para que no se salga de la pantalla)
            posx-=2; if(posx<=LIMITE_IZQ) posx=LIMITE_IZQ;
            SPR_setAnim(Nave, ANIM_NAVE_MOV);
        }
        if(value & BUTTON_RIGHT)
        {
            posx+=2; if(posx>=LIMITE_DCH) posx=LIMITE_DCH;
            SPR_setAnim(Nave, ANIM_NAVE_MOV);
        }
        if (value & BUTTON_UP)
        {
            posy-=2; if(posy<=LIMITE_UP) posy=LIMITE_UP;
            SPR_setAnim(Nave, ANIM_NAVE_MOV);
        }
        if(value & BUTTON_DOWN)
        {
            posy+=2; if(posy>=LIMITE_DOWN) posy=LIMITE_DOWN;
            SPR_setAnim(Nave, ANIM_NAVE_MOV);
        }

        //si no pulsamos, animacion idle
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)) &&
            (!(value & BUTTON_UP)) && (!(value & BUTTON_DOWN)))
            SPR_setAnim(Nave, ANIM_NAVE_IDLE);

        //reinicia la ROM
        if (value & BUTTON_START) SYS_hardReset();  //hay que añadir #include "sys.h" en la cabecera

        //actualiza posicion del sprite
        SPR_setPosition(Nave, posx, posy);
    }
}



static void muestraMENSAJES()
{
    if(estado == 1) //estado inicial al empezar el programa
    {
    VDP_drawText("Ejemplo 01", 4, 12);
    VDP_drawText("Pulsa -A- para continuar", 4, 13);
    }

    if(estado == 2)
    {
    VDP_drawText("Ejemplo 02", 4, 12);
    VDP_drawText("Pulsa -B- para continuar", 4, 13);
    }

    if(estado == 3)
    {
    VDP_drawText("Ejemplo 03", 4, 12);
    VDP_drawText("Pulsa -C- para continuar", 4, 13);
    }

    if(estado == 4)
    {
    VDP_drawText("Ejemplo 04", 4, 12);
    VDP_drawText("Pulsa -A- para continuar", 4, 13);
    }

    if(estado == 5)
    {
    VDP_drawText("Ejemplo 05", 4, 12);
    VDP_drawText("Pulsa -B- para continuar", 4, 13);
    }

    if(estado == 6)
    {
    VDP_drawText("Ejemplo 06", 4, 12);
    VDP_drawText("Pulsa -C- para continuar", 4, 13);
    }

    if(estado == 7)
    {
    VDP_drawTextBG(PLAN_B,"Ejemplo 07", 14, 24);
    VDP_drawTextBG(PLAN_B,"Pulsa -START- para REINICIAR", 14, 25);
    }

}
