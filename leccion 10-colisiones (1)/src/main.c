/**
 *      @Title:  Leccion 10 - "Colisiones (1)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include "sprites.h"  //carga sprites

//Constantes
    //primera parte
    #define TOLERANCIA 10   //distancia a partir de la cual hay colision (de cero a TOLERANCIA)
    //segunda parte
    #define SONIC_IDLE      0
    #define SONIC_IDLE2     1
    #define SONIC_IDLE3     2
    //tercera parte
    #define MAX_BALAS       10  //num de balas en pantalla

//Declaracion de funciones

    //primera parte
    static void handleInput();
    static void muestra_posiciones();
    void mi_hblank();
    //segunda parte
    static void chequea_colision1();
    static void chequea_colision2();
    static void handleInput_parte2();
    //tercera parte
    static void chequea_colision3();
    static void chequea_colision4();
    static void handleInput_parte3();


//Declaracion de variables

    //primera parte

    //Punteros a sprite
    Sprite* mi_bala;
    Sprite* otra_bala;

    //Posicion inicial en pantalla de los sprites
    int bala_posx = 64;
    int bala_posy = 145;
    int bala2_posx = 100;
    int bala2_posy = 100;

    //(0: colision HW, 1: colision POR TOLERANCI)
    int seleccion_tipo_colision = 0;

    //segunda parte

    //Punteros a sprite
    Sprite* mi_sonic;

    //Posicion inicial en pantalla de los sprites
    int sonic_posx = 200;
    int sonic_posy = 85;

    //Caja de colisiones
    //4 puntos que definen un rectangulo con el cual vamos a
    // detectar colision si la bala está dentro de él
    struct{

        int x1, y1, x2, y2;

    }CajaColision;

    //tercera parte
    Sprite* mi_MuchasBalas[MAX_BALAS];
    fix32 MuchasBalas_posx[MAX_BALAS];
    fix32 MuchasBalas_posy[MAX_BALAS];







int main()
{

    /****************** PARTE 1: COLISION HW y por DISTANCIA ******************/

    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //recoge la paleta de la bala
    VDP_setPalette(PAL1,bala_sprite.palette->data);

    //añade los dos sprites de las balas
    mi_bala   = SPR_addSprite(&bala_sprite, bala_posx,  bala_posy,  TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    otra_bala = SPR_addSprite(&bala_sprite, bala2_posx, bala2_posx, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    //AYUDA en pantalla
    VDP_drawText("COLISIONES HW y por DISTANCIA   ",  2, 3);
    VDP_drawText("A - Colision bit HW del VDP     ",  2, 4);
    VDP_drawText("B - Colision por DISTANCIA      ",  2, 5);
    VDP_drawText("Pulsa - START - para continuar  ",  2, 26);

    SYS_setHIntCallback(mi_hblank); //Define la funcion a lanzar cuando se de la interrupcion
    VDP_setHIntCounter(8);		    //Cada 8 scanlines, se lanzará la interrupcion horizontal
    VDP_setHInterrupt(1);		    //Activa la interrupcion horizontal (comenzamos buscando colision HW)

    //Bucle principal primera parte
    while(TRUE)
    {
        //recoge la entrada de los mandos
        handleInput();
            //si pulsamos START salimos del bucle y vamos a la parte2
            u16 value = JOY_readJoypad(JOY_1);
            if (value & BUTTON_START) break;

        //Colision HW
        if(seleccion_tipo_colision == 0)
        {
            VDP_drawText("Bit Colision HW?     ",  2, 20);
            if(GET_VDPSTATUS(VDP_SPRCOLLISION_FLAG)!=0) VDP_drawText("Si", 26, 20);
            else VDP_drawText("No", 26, 20);
        }
        //Colision por DISTANCIA
        if(seleccion_tipo_colision == 1)
        {
            VDP_drawText("En rango de colision?",  2, 20);
            if( abs(bala_posx-bala2_posx)<TOLERANCIA && abs(bala_posy-bala2_posy)<TOLERANCIA)
                VDP_drawText("Si", 26, 20);
            else
                VDP_drawText("No", 26, 20);
        }

        //muestra las coordenadas de los sprites
        muestra_posiciones();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }


    /****************** PARTE 2: CAJAS DE COLISION ******************/

    //desactiva cualquier interrupcion (colision HW)
    SYS_setHIntCallback(NULL);
    VDP_setHIntCounter(NULL);
    VDP_setHInterrupt(NULL);
    //se carga los sprites anteriores
    SPR_reset();
    SPR_init(0,0,0);

    //borro textos anteriores
    VDP_clearTextAreaBG(PLAN_A,0,20,40,28);

    //carga los recursos necesarios para esta parte

    //añade el sprite de la bala
    bala_posx = 100;
    bala_posy = 100;
    mi_bala   = SPR_addSprite(&bala_sprite, bala_posx,  bala_posy,  TILE_ATTR(PAL1, TRUE, FALSE, FALSE));


    //recoge la paleta de sonic y la mete en la 3a paleta del sistema
    VDP_setPalette(PAL2,sonic_sprite.palette->data);

    //carga el sprite de Sonic
    mi_sonic = SPR_addSprite(&sonic_sprite, sonic_posx, sonic_posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE2);

    //1: colision puntos de origen, 2: colision con caja de colisiones
    seleccion_tipo_colision = 1;

    //AYUDA en pantalla
    VDP_drawText("COLISION simple                ",  2, 3);
    VDP_drawText("A - Colisiones: Punto Origen   ",  2, 4);
    VDP_drawText("B - Colisiones: Caja colisiones",  2, 5);
    VDP_drawText("Pulsa - C - para continuar  ",  2, 26);

    //Bucle principal segunda parte
    while(TRUE)
    {
        //recoge la entrada de los mandos
        handleInput_parte2();
            //si pulsamos C salimos del bucle y vamos a la parte3
            u16 value = JOY_readJoypad(JOY_1);
            if (value & BUTTON_C) break;

        //Chequeo de colisiones
        if(seleccion_tipo_colision == 1) chequea_colision1();
        if(seleccion_tipo_colision == 2) chequea_colision2();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }


    /****************** PARTE 3: MULTI COLISION  ******************/
    //se carga los sprites anteriores
    SPR_reset();
    SPR_init(0,0,0);

    //borro textos anteriores
    VDP_clearTextAreaBG(PLAN_A,0,20,40,28);

    //carga los recursos necesarios para esta parte
    int cont;
    for(cont = 0; cont < MAX_BALAS; cont++)
    {
        MuchasBalas_posx[cont] = random() % 320;   // random() : Return a random u16 integer (0..65535).
        MuchasBalas_posy[cont] = random() % 224;   // % operador modulo :  devuelve el resto de una division (siempre un ENTERO)
                                            // en este caso devuelve de 0 a la cantidad indicada
        mi_MuchasBalas[cont]  = SPR_addSprite(&bala_sprite,   MuchasBalas_posx[cont],  MuchasBalas_posy[cont], TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    }

    //sprite sonic
    sonic_posx = 100;
    sonic_posy = 100;
    mi_sonic = SPR_addSprite(&sonic_sprite, sonic_posx, sonic_posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE2);

    //AYUDA en pantalla
    VDP_drawText("MULTI COLISIONES                     ",  2, 3);
    VDP_drawText("A - Colisiones: bucle Punto Origen   ",  2, 4);
    VDP_drawText("B - Colisiones: bucle Caja colisiones",  2, 5);

    //por defecto
    seleccion_tipo_colision = 1;

    //Bucle principal tercera parte
    while(TRUE)
    {
        //recoje la entrada de los mandos
        handleInput_parte3();

        //Chequeo de colisiones
        if(seleccion_tipo_colision == 1) chequea_colision3();
        if(seleccion_tipo_colision == 2) chequea_colision4();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }



    return 0;
}












/********** FUNCIONES DE LA PRIMERA PARTE ****************/

void mi_hblank()
{
    //lee el bit del VDP y poco más, la cuestión es que hay que leerlo en la interrupcion
    //si se intenta leer fuera de aqui, no dara resultado alguno
    if(GET_VDPSTATUS(VDP_SPRCOLLISION_FLAG)!=0) VDP_drawText("Si", 26, 20);
}

//Funcion handleInput()  recoge la entrada del mando y actualiza la posicion de Sonic
//ademas con A,B,C selecciona deteccion de colisiones
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(mi_bala, bala_posx--, bala_posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_bala, bala_posx++, bala_posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_bala, bala_posx, bala_posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_bala, bala_posx, bala_posy++);

    //si pulsamos A
    if (value & BUTTON_A){
        seleccion_tipo_colision = 0;

        SYS_setHIntCallback(mi_hblank);
        VDP_setHIntCounter(8);		//cada 8 scanlines, se lanzará la interrupcion horizontal
        VDP_setHInterrupt(1);		//Enable horizontal interrupt
    }
    //si pulsamos B
    if (value & BUTTON_B){
        seleccion_tipo_colision = 1;

        //desactiva cualquier interrupcion (colision HW)
        SYS_setHIntCallback(NULL);
        VDP_setHIntCounter(NULL);
        VDP_setHInterrupt(NULL);
    }

}

static void muestra_posiciones()
{
    //"%4d" justific a derechas, si no se pone al poner el contador a 0
    //los numeros nuevos (1 caracter:0,1,2...) se machacan/mezclan con
    //los viejos (que tienen hasta 3 caracteres:   999)

    //posicion bala1
    char x1_string[32];
    sprintf(x1_string, "%4d", bala_posx);
    VDP_drawText("bala X:",  2, 21);
    VDP_drawText(x1_string, 14, 21);
    char y1_string[32];
    sprintf(y1_string, "%4d", bala_posy);
    VDP_drawText("bala Y:",  2, 22);
    VDP_drawText(y1_string, 14, 22);

    //posicion bala2
    char x2_string[32];
    sprintf(x2_string, "%4d", bala2_posx);
    VDP_drawText("Bala2 X:",  2, 23);
    VDP_drawText(x2_string,  14, 23);
    char y2_string[32];
    sprintf(y2_string, "%4d", bala2_posy);
    VDP_drawText("Bala2 Y:",  2, 24);
    VDP_drawText(y2_string,  14, 24);

}


/********** FUNCIONES DE LA SEGUNDA PARTE ****************/

//Recoge la entrada del mando y actualiza la posicion de Sonic
//ademas con A,B,C selecciona deteccion de colisiones
static void handleInput_parte2()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(mi_sonic, sonic_posx--, sonic_posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_sonic, sonic_posx++, sonic_posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_sonic, sonic_posx, sonic_posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_sonic, sonic_posx, sonic_posy++);

    //si pulsamos B
    if (value & BUTTON_A){
        SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE2); seleccion_tipo_colision = 1;
    }
    //si pulsamos B
    if (value & BUTTON_B){
        SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE3); seleccion_tipo_colision = 2;
    }
}


//CHEQUEA COLISION: A PARTIR DE PUNTOS DE ORIGEN
static void chequea_colision1()
{
    //Comprobando colisiones
    VDP_drawText("En rango de colision?",  2, 20);

    if( abs(sonic_posx-bala_posx)<TOLERANCIA && abs(sonic_posy-bala_posy)<TOLERANCIA)
        VDP_drawText("Si", 26, 20);
    else
        VDP_drawText("No", 26, 20);

    KLog_U1("sonic_posx:", sonic_posx);
    KLog_U1("bala_posx :", bala_posx);
    KLog_U1("resta     :", abs(sonic_posx-bala_posx));
    KLog_U1("  sonic_posy:", sonic_posy);
    KLog_U1("  bala_posy :", bala_posy);
    KLog_U1("  resta     :", abs(sonic_posy-bala_posy));


    //"%4d" justific a derechas, si no se pone al poner el contador a 0
    //los numeros nuevos (1 caracter:0,1,2...) se machacan/mezclan con
    //los viejos (que tienen hasta 3 caracteres:   999)

    //posicion Sonic
    char x1_string[32];
    sprintf(x1_string, "%4d", sonic_posx);
    VDP_drawText("Sonic X:",  2, 21);
    VDP_drawText(x1_string, 16, 21);
    char y1_string[32];
    sprintf(y1_string, "%4d", sonic_posy);
    VDP_drawText("Sonic Y:",  2, 22);
    VDP_drawText(y1_string, 16, 22);

    //posicion bala
    char x2_string[32];
    sprintf(x2_string, "%4d", bala_posx);
    VDP_drawText("Bala X:",  2, 23);
    VDP_drawText(x2_string, 16, 23);
    char y2_string[32];
    sprintf(y2_string, "%4d", bala_posy);
    VDP_drawText("Bala Y:",  2, 24);
    VDP_drawText(y2_string, 16, 24);
}

//CHEQUEA COLISION: CON CAJA DE COLISION -> NO HAY TOLERANCIA
static void chequea_colision2()
{
    //actualiza la caja de colision
    CajaColision.x1 = sonic_posx + 16;
    CajaColision.y1 = sonic_posy +  8;
    CajaColision.x2 = sonic_posx + 31;
    CajaColision.y2 = sonic_posy + 31;

    //Comprobando colisiones
    VDP_drawText("En rango de colision?",  2, 20);

    if( bala_posx>=CajaColision.x1 && bala_posx<=CajaColision.x2 && bala_posy>=CajaColision.y1 && bala_posy<=CajaColision.y2 )
    {

        VDP_drawText("Si", 26, 20);

    } else VDP_drawText("No", 26, 20);


    //"%4d" justific a derechas, si no se pone al poner el contador a 0
    //los numeros nuevos (1 caracter:0,1,2...) se machacan/mezclan con
    //los viejos (que tienen hasta 3 caracteres:   999)

    //posicion Sonic
    char x1_string[32];
    sprintf(x1_string, "%4d", sonic_posx);
    VDP_drawText("Sonic X:",  2, 21);
    VDP_drawText(x1_string, 16, 21);
    char y1_string[32];
    sprintf(y1_string, "%4d", sonic_posy);
    VDP_drawText("Sonic Y:",  2, 22);
    VDP_drawText(y1_string, 16, 22);
}



/********** FUNCIONES DE LA TERCERA PARTE ****************/

//Funcion handleInput()  recoje la entrada del mando y actualiza la posicion de Sonic
//ademas con A,B,C selecciona deteccion de colisiones
static void handleInput_parte3()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(mi_sonic, sonic_posx--, sonic_posy);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(mi_sonic, sonic_posx++, sonic_posy);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
        SPR_setPosition(mi_sonic, sonic_posx, sonic_posy--);
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
        SPR_setPosition(mi_sonic, sonic_posx, sonic_posy++);

    //si pulsamos B
    if (value & BUTTON_A){
        SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE2); seleccion_tipo_colision = 1;
    }
    //si pulsamos C
    if (value & BUTTON_B){
        SPR_setAnimAndFrame(mi_sonic, 0, SONIC_IDLE3); seleccion_tipo_colision = 2;
    }
}

//CHEQUEA COLISION: A PARTIR DE PUNTOS DE ORIGEN
static void chequea_colision3()
{
    //contador
    int cont, cont2;

    //Comprobando colisiones
    VDP_drawText("comprobando colisiones?",  2, 22);

    for(cont = 0, cont2 = 0; cont < MAX_BALAS; cont++)
    {
        if( abs(sonic_posx-MuchasBalas_posx[cont])<TOLERANCIA &&
            abs(sonic_posy-MuchasBalas_posy[cont])<TOLERANCIA)
            cont2++;
    }

    if(cont2==0) VDP_drawText("No", 28, 22); else VDP_drawText("Si", 28, 22);
/*
    //posicion Sonic
    //por algún motivo... si no comentamos esto de aquí abajo, va más lento de lo normal
    //(pero NO en chequea_colision2() donde hacemos exactamente lo mismo  ?? )
    char x1_string[32];
    sprintf(x1_string, "%4d", sonic_posx);
    VDP_drawText("Sonic X:",  2, 21);
    VDP_drawText(x1_string, 16, 21);
    char y1_string[32];
    sprintf(y1_string, "%4d", sonic_posy);
    VDP_drawText("Sonic Y:",  2, 22);
    VDP_drawText(y1_string, 16, 22);*/

}



//CHEQUEA COLISION: CON CAJA DE COLISION -> NO HAY TOLERANCIA
static void chequea_colision4()
{
    //para contar y para ver si hay, al menos, una colision
    int cont, cont2;

    //actualiza la caja de colision
    CajaColision.x1 = sonic_posx + 16;
    CajaColision.y1 = sonic_posy +  8;
    CajaColision.x2 = sonic_posx + 31;
    CajaColision.y2 = sonic_posy + 31;

    //Comprobando colisiones
    VDP_drawText("comprobando colisiones?",  2, 22);

    for(cont = 0, cont2 = 0; cont < MAX_BALAS; cont++)
    {
        if( MuchasBalas_posx[cont]>=CajaColision.x1 && MuchasBalas_posx[cont]<=CajaColision.x2 &&
            MuchasBalas_posy[cont]>=CajaColision.y1 && MuchasBalas_posy[cont]<=CajaColision.y2 )
        {
            cont2++;
        }
    }

    if(cont2==1) VDP_drawText("Si", 28, 22); else VDP_drawText("No", 28, 22);

}
