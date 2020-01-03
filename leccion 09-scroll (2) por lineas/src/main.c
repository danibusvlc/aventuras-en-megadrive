/**
 *      @Title:  "leccion 09 - "scroll (2) por lineas"
 *      @Author: Daniel Bustos "danibus"
 */

/*
    EJEMPLO 01: Scroll por lineas diversos tipos
    EJEMPLO 02: Suelo tipo SF2
    EJEMPLO 03: Efectos con el logo de SEGA
*/

#include <genesis.h>

#include "gfx.h"     //carga las imágenes de background
#include "sprite.h"  //carga los sprites
#include "sys.h"     //para hacer el reset

#define ANIM_STAND   0   //animaciones de Ryu (EJEMPLO 02)
#define ANIM_WALK    1



//Declaracion de funciones
static void handleInput();
static void cargaESTADO();
static void muestraMENSAJES();
static void actualizaCamara();



//Declaracion de variables

    //Cuenta de tiles en VRAM
    u16 ind;

    //Para ir pasando por los distintos ejemplos
    int estado  = 1;
    int cerrojo = 0;

    //ejemplo 01
    s16 offsetA = 0;         //desplazamiento plano A
    s16 vectorA[224];        //todo el plano A
    s16 vectorB1[50];        //de 0 a 50
    s16 vectorB2[80];        //144 a 224px
    fix16 vectorB2_aux[80];            //vector auxiliar
    fix16 vectorB2_ace = FIX16(0.6);   //constante para la aceleracion

    //ejemplo 02
    Sprite *spr_ryu;                //sprite RYU
    u16 spr_posx = 160;             //posicion del sprite
    u16 spr_posy = 120;
    #define LIMITE_IZQ           70 //limites de movimiento del sprite
    #define LIMITE_DCHO         260

    fix16 offsetB;                              //desplazamiento plano B (muy lento)
    s16 vectorB[224];                           //todo el plano B
    fix16 vectorB_aux[224];                     //vector auxiliar
    #define VELOCIDAD_FONDO_B    FIX16(  0.65)  //velocidad desplazamiento plano B
    #define VELOCIDAD_FONDO_BN   FIX16( -0.65)

    //nota: para el plano A re-uso  algunas vbles del ejemplo 01


    //vectores para el manejo del SUELO tipo SF2
    // vectorS: vector de enteros, se utiliza para pasar los datos de scroll a VDP_setHorizontalScrollLine (que solo admite enteros)
    // vectorS_ace: vector de aceleracion, define para cada línea del suelo cuando debe moverse en cada ciclo, la parte superior del suelo
    //              se movera más lentamente que la inferior. Se ha sacado A OJO comparando con el juego original.
    // vectorS_aux: vector auxiliar sobre el que se realizan los calculos.

    s16 vectorS[40];
    fix16 vectorS_ace[40] ={0.8,1.0,1.2,1.6, 2.0,  2.4, 2.8, 3.2, 3.6, 4.0,   4.4, 4.8, 5.2, 5.6, 6.0,  6.4, 6.8, 7.2, 7.6, 8.0,
                            8.4,8.8,9.2,9.6,10.0, 10.4,10.8,11.2,11.6,12.0,  12.0,12.0,12.0,12.0,12.0, 12.0,12.0,12.0,12.0,12.0};
    fix16 vectorS_aux[40];


    //ejemplo 03: mismo codigo que en el ejemplo "leccion 08-shadow & highlight (5) Castlevania"

    //// CONSTANTES Y METODOS PARA DIAGONALES ////////////

        // Some data to deal with graphical data
        #define NUM_COLUMNS     40
        #define NUM_ROWS        28
        #define NUM_LINES       NUM_ROWS * 8
        //Pone a 0 todos los elementos de line_scroll_data[]
        #define InitializeScrollTable(); \
            for(u16 i = 0; i < NUM_LINES; i++) line_scroll_data[i] = FIX16(0);
        //Pone la sig secuencia (0,1,2,3...223) en line_speed_data[]
        #define InitializeSpeedTable(); \
            line_speed_data[0] = FIX16(0); \
            for(u16 i = 1; i < NUM_LINES; i++) \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], FIX16(1));
        //idem pero con diagonal dinamica
        #define InitializeSpeedTable_dinamica(); \
            line_speed_data[0] = FIX16(0); \
            for(u16 i = 1; i < NUM_LINES; i++){ \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], FIX16(1)); \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], diagonal_dinamica); }
        //para la diagonal dinamica
        fix16 diagonal_dinamica=FIX16(0);


    //ejemplo 04: separacion por lineas
    s16 vSeparacion[224];
    fix16 vSeparacion_ace[224];
    fix16 vSeparacion_aux[224];


//MAIN
int main()
{
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
    //primero lo resetea todo, quizás esta no es la forma más elegante
    SPR_end();
    VDP_resetScreen();
    VDP_init();


    //pone algunos vectores a sus valores iniciales (cambian durante los ejemplos)
    if(estado==1)
    {
        offsetA = 0;
        for(int i = 0; i < 224; i++) vectorA[i]  = 0;
        for(int i = 0; i < 50;  i++) vectorB1[i] = 0;
        for(int i = 0; i < 80;  i++) vectorB2[i] = 0;
        for(int i = 0; i < 80;  i++) vectorB2_aux[i]= FIX16(0);

        //320x224px
        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        //paletas
        VDP_setPalette(PAL0, bgb_image.palette->data);
        VDP_setPalette(PAL1, bga_image.palette->data);
        VDP_setPalette(PAL2, ryu_sprite.palette->data);

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //backgrounds
        ind = TILE_USERINDEX;
        VDP_drawImageEx(PLAN_B, &bgb_image, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bgb_image.tileset->numTile;
        VDP_drawImageEx(PLAN_A, &bga_image, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bga_image.tileset->numTile;

        //Configura el scroll (por LINEA)
        VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
    }

    if(estado==2)
    {
        offsetA = 0; offsetB = FIX16(0);
        for(int i = 0; i < 224; i++) vectorA[i]     = -96;
        for(int i = 0; i < 224; i++) vectorB[i]     = FIX16(0);
        for(int i = 0; i < 224; i++) vectorB_aux[i] = FIX16(-56);

        for(int i = 0; i < 40;  i++) vectorS[i]     = -96;
        for(int i = 0; i < 40;  i++) vectorS_aux[i] = FIX16(-96.0);

        //320x224px
        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        //paletas
        VDP_setPalette(PAL0, bgc_image.palette->data);  //ambos fondos usan esta paleta
        VDP_setPalette(PAL1, ryu_sprite.palette->data); //ryu

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //backgrounds
        ind = TILE_USERINDEX;
        VDP_drawImageEx(PLAN_B, &bgd_image, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bgd_image.tileset->numTile;
        VDP_drawImageEx(PLAN_A, &bgc_image, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bgc_image.tileset->numTile;

        //Configura el scroll (por LINEA)
        VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

        //sprite
        spr_ryu = SPR_addSprite(&ryu_sprite, spr_posx, spr_posy, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
        SPR_setHFlip(spr_ryu, TRUE);
    }


    if(estado==3)
    {
        //320x224px
        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        //paletas
        VDP_setPalette(PAL0, bge_image.palette->data);

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //backgrounds
        ind = TILE_USERINDEX;
        VDP_drawImageEx(PLAN_A, &bge_image, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bge_image.tileset->numTile;

        //Configura el scroll (por LINEA)
        VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);
    }


    if(estado==4)
    {

        for(int i = 0; i < 224;  i++)         vSeparacion[i]     = 0;
        //lineas pares una velocidad, lineas impares la contraria
        for(int i = 0; i < 224;  i++) if(i%2) vSeparacion_ace[i] = FIX16(0.05); else  vSeparacion_ace[i] = FIX16(-0.05);
        for(int i = 0; i < 224;  i++)         vSeparacion_aux[i] = FIX16(0);

        //320x224px
        VDP_setScreenWidth320();
        VDP_setScreenHeight224();

        //inicializa motor de sprites
        SPR_init(0, 0, 0);

        //paletas
        VDP_setPalette(PAL0,bge_image.palette->data);
        //backgrounds
        ind = TILE_USERINDEX;
        VDP_drawImageEx(PLAN_A, &bge_image, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
        ind += bge_image.tileset->numTile;

        //Configura el scroll (por LINEA)
        VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

    }



    //finalmente activamos el cerrojo para no salirse del bucle secundario
    cerrojo = 1;

}



static void actualizaCamara()
{
    if(estado == 1)
    {
        //plano A
        for(int i = 0; i < 224; i++) vectorA[i] = offsetA;
        VDP_setHorizontalScrollLine(PLAN_A,  0, vectorA, 224, CPU);
        offsetA--;
        //plano B: nubes
        for(int i = 0; i < 50; i++)  vectorB1[i] -= 2;
        VDP_setHorizontalScrollLine(PLAN_B,  0, vectorB1,  50, CPU);
        //plano B: mar
        for(int i = 0; i < 80; i++){
            vectorB2_aux[i] = fix16Sub(vectorB2_aux[i],vectorB2_ace+i);
            vectorB2[i] = fix16ToInt(vectorB2_aux[i]);
        }
        VDP_setHorizontalScrollLine(PLAN_B,144, vectorB2, 80, CPU);
    }

    if(estado == 2)
    {
        //plano A (todo lo que no es el suelo)
        for(int i = 0; i < 180; i++) vectorA[i] += offsetA;
        VDP_setHorizontalScrollLine(PLAN_A,    0, vectorA, 180, CPU);
        //plano A (suelo)
        VDP_setHorizontalScrollLine(PLAN_A,  180, vectorS, 40, CPU);
        //plano B (completo)
        for(int i = 0; i < 224; i++){
            vectorB_aux[i] = fix16Add(vectorB_aux[i],offsetB);
            vectorB[i] = fix16ToInt(vectorB_aux[i]);
        }
        VDP_setHorizontalScrollLine(PLAN_B,    0, vectorB, 224, CPU);
    }

    if(estado == 3)
    {
        //en este ejemplo no hacemos nada desde aqui
    }

    if(estado == 4)
    {
        //plano A
        VDP_setHorizontalScrollLine(PLAN_A,  0, vectorS, 224, CPU);
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

        //si pulsamos izquierda...
        if (value & BUTTON_LEFT)
        {
            if(spr_posx >= LIMITE_IZQ)
            {
                SPR_setPosition(spr_ryu, spr_posx-=2, spr_posy);
                SPR_setAnim(spr_ryu, ANIM_WALK);
                SPR_setHFlip(spr_ryu, FALSE);
                offsetA=+1;                 //plano A (todo lo que no es el suelo)
                offsetB=VELOCIDAD_FONDO_B;  //plano B

                //plano A (suelo)
                for(int i = 0; i < 40; i++){
                    vectorS_aux[i] = fix16Sub(vectorS_aux[i],vectorS_ace[i]); //aceleracion para el suelo
                    vectorS_aux[i] = fix16Add(vectorS_aux[i],FIX16(+1));      //mas el desplazamiento del fondo
                    vectorS[i] = fix16ToInt(vectorS_aux[i]);
                }

            }else{offsetA = 0; offsetB = FIX16(0);}
        }

        //si pulsamos derecha...
        if (value & BUTTON_RIGHT)
        {
            if(spr_posx <= LIMITE_DCHO)
            {
                SPR_setPosition(spr_ryu, spr_posx+=2, spr_posy);
                SPR_setAnim(spr_ryu, ANIM_WALK);
                SPR_setHFlip(spr_ryu, TRUE);
                offsetA=-1;                    //plano A (todo lo que no es el suelo)
                offsetB=VELOCIDAD_FONDO_BN;    //plano B

                //plano A (suelo)
                for(int i = 0; i < 40; i++){
                    vectorS_aux[i] = fix16Add(vectorS_aux[i],vectorS_ace[i]); //aceleracion para el suelo
                    vectorS_aux[i] = fix16Add(vectorS_aux[i],FIX16(-1));       //mas el desplazamiento del fondo
                    vectorS[i] = fix16ToInt(vectorS_aux[i]);
                }

            }else{offsetA = 0; offsetB = FIX16(0);}
        }

        //si no pulsamos
        if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
        {
            SPR_setAnim(spr_ryu, ANIM_STAND);
            offsetA = 0; offsetB = FIX16(0);
        }
    }

    if(estado == 3) //deformacion logo = ejemplo diagonales de Entrada Castlevania
    {
        if (value & BUTTON_C) cerrojo=0;

        if(value & BUTTON_RIGHT)
        {
            //incrementamos la diagonal
            diagonal_dinamica = fix16Add(diagonal_dinamica,FIX16(0.025));
            //como antes
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];
            //en este caso la tabla cambia
            InitializeScrollTable();
            InitializeSpeedTable_dinamica();

                for(u16 i = 0; i < NUM_LINES; i++)
                {
                    line_scroll_data[i] = fix16Add(line_scroll_data[i], line_speed_data[i]);
                    aux[i] = fix16ToInt(line_scroll_data[i]);
                }

                VDP_setHorizontalScrollLine(PLAN_A, 0, aux, NUM_LINES, 1);
        }

        if(value & BUTTON_LEFT)
        {
            //incrementamos la diagonal
            diagonal_dinamica = fix16Sub(diagonal_dinamica,FIX16(0.025));
            //como antes
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];
            //en este caso la tabla cambia
            InitializeScrollTable();
            InitializeSpeedTable_dinamica();

                for(u16 i = 0; i < NUM_LINES; i++)
                {
                    line_scroll_data[i] = fix16Add(line_scroll_data[i], line_speed_data[i]);
                    aux[i] = fix16ToInt(line_scroll_data[i]);
                }

                VDP_setHorizontalScrollLine(PLAN_A, 0, aux, NUM_LINES, 1);
        }
    }


    if(estado == 4) //separacion por lineas
    {

        if (value & BUTTON_START) SYS_hardReset();  //reinicia la ROM, hay que añadir #include "sys.h" en la cabecera

        if(value & BUTTON_RIGHT)
        {
                //
                for(int i = 0; i < 224; i++){
                    vSeparacion_aux[i] = fix16Add(vSeparacion_aux[i],vSeparacion_ace[i]);
                    vSeparacion[i] = fix16ToInt(vSeparacion_aux[i]);
                }
        }
        if(value & BUTTON_LEFT)
        {
                //
                for(int i = 0; i < 224; i++){
                    vSeparacion_aux[i] = fix16Sub(vSeparacion_aux[i],vSeparacion_ace[i]);
                    vSeparacion[i] = fix16ToInt(vSeparacion_aux[i]);
                }
        }
    }

}







static void muestraMENSAJES()
{

    if(estado == 1) //estado inicial al empezar el programa
    {
    VDP_drawText("Ejemplo 01", 18, 10);
    VDP_drawText("Pulsa -A- para continuar", 18, 11);
    }

    if(estado == 2)
    {
    VDP_drawText("Ejemplo 02", 18, 10);
    VDP_drawText("LEFT/RIGHT: Movimiento", 18, 12);
    VDP_drawText("Pulsa -B- para continuar", 18, 13);
    }

    if(estado == 3)
    {
    VDP_drawText("Ejemplo 03", 10, 22);
    VDP_drawText("LEFT/RIGHT: Diagonal", 10, 23);
    VDP_drawText("Pulsa -C- para continuar", 10, 24);
    }

    if(estado == 4)
    {
    VDP_drawText("Ejemplo 04", 10, 22);
    VDP_drawText("LEFT/RIGHT: Separacion", 10, 23);
    VDP_drawText("Pulsa -START- para REINICIAR", 10, 24);
    }
}
