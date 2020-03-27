/**
 *      @Title:  "Leccion 11 - Escenarios interactivos"
 *      @Author: Daniel Bustos "danibus"
 */


/*
    EJEMPLO 0: Tintado usando paleta de rojos (PAL3)
    EJEMPLO 1: Tintado LENTO usando paleta de rojos (PAL3)
    EJEMPLO 2: Tintado usando solo 1 color (color 9 de la paleta 0)
    EJEMPLO 3: Tintado usando una paleta usando (PAL1)
    EJEMPLO 4: Borrado completo de un plano
    EJEMPLO 5: Borrado parcial de un plano
    EJEMPLO 6: Torres destructibles: Cambio de tiles
*/

#include <genesis.h>
#include "sprite.h"
#include "fondos.h"
#include "sys.h"

#define FUERA_PANTALLA      -100    //para colocar cosas fuera de la zona visible de la TV
#define MAX_TORRES          2       //num de torres
#define T_TINTE             2       //tiempo durante el cual sprite/escenario se pone rojo al recibir colision
#define T_TINTE_LENTO       9       //tiempo durante el cual sprite/escenario se pone rojo al recibir colision
#define POS_MIRA            8       //Posición disparo respecto a la mira

//declaracion de funciones

    void joyEvent(u16 joy, u16 changed, u16 state);

    void crea_nivel();
    void crea_decorado();
    void maneja_decorado();
    void crea_mira();
    void maneja_mira();

    void crea_bala_player();
    void activa_bala_player();
    void maneja_bala_player();

    void detecta_colision();

    void muestraMENSAJES();
    void pinta_vida();


//declaracion de variables
int cerrojo = 1;  //para ir viendo los distintos ejemplos
int ejemplo = 0;  //ejemplo mostrado

    struct {
        s16 x,y ;           //posicion
        s16 movx;           //Velocidad mov eje x
        s16 movy;           //Velocidad mov eje y
        Sprite *spr_mira;   //grafico de la mira
    }Mira;

    struct{
        Sprite* spr_bala;
        s16 x,y;            //posicion
        int esta_activo;    //0=bala "muerta"="sin uso" / del 1 al 4 activo y corresponde a los 4 frames del disparo
    }DisparoPlayer;

    struct {
        int vida;           //puntos vida
        int estado;         //estado (0=inicial, 1=semidestruida, 2= cimientos)
        s16 colX1,colY1;    //caja colision torre (punto sup izq)
        s16 colX2,colY2;    //caja colision torre (punto inf dch)
        s16 activa_impacto; //para mostrar impactos
        s16 t_impacto;      //para mostrar impactos
    }Torre[2];

    Map *Estructuras;        // para el MAP de las Torres

	u16 id_tile_inicial;    // siempre será 1 (el tile 0 se lo reserva el SGDK)
	u16 id_tile_final_planB;// apunta al primer tile libre DESPUES de cargar en mem VDP los tiles del plan B
	u16 id_tile_final_planA;// apunta al primer tile libre DESPUES de cargar en mem VDP los tiles del plan A




int main() {

    //320x224
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    //Recoge la paleta del fondo y de las torres
    VDP_setPalette(PAL0,fondo01_01.palette->data);
    //Recoge la paleta de la mira, de los disparos y del texto
    VDP_setPalette(PAL1,sprite_mira.palette->data);
    //Paleta 3 a ROJOS, para el ejemplo correspondiente.
    //    ( la PAL2 no la inicializo ni uso, el SGDK la pondrá de forma automática a verdes )
    VDP_setPalette(PAL3, palette_red);
    //Inicia el motor de sprites
    SPR_init();
    //creacion de elementos
    crea_nivel();         //crea el nivel
    crea_bala_player();   //crea el disparo (antes que la mira para salir por delante)
    crea_mira();          //crea la  mira
    //deteccion asincrona de pulsaciones (para cambiar de ejemplo)
    JOY_setEventHandler(joyEvent);

    //BUCLE PRINCIPAL
	while (TRUE)
    {
        crea_decorado();      //re-crea el decorado, las torres (se hace al final para salir por detras)
        muestraMENSAJES();    //ayuda en pantalla

        //BUCLE SECUNDARIO (una vez por ejemplo)
        while(cerrojo)
        {
            maneja_decorado();
            maneja_mira();
            trata_bala_player();
            pinta_vida();
            //actualiza VDP
            SPR_update();
            //Espera al barrido vertical TV
            VDP_waitVSync();
        }

        ejemplo++;
    }

return 1;
}







//CREA LOS FONDOS INICIALES
void crea_nivel(){

    //Desactiva las interrupciones
    SYS_disableInts();

    //Borra los ambos planos
    VDP_clearPlan(PLAN_A, TRUE);
    VDP_clearPlan(PLAN_B, TRUE);

    //FONDOS
    id_tile_inicial = 1;
    //PLANO B: fondo castillo
    VDP_drawImageEx(PLAN_B, &fondo01_01,  TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_inicial), 0, 0, FALSE, TRUE);
    id_tile_final_planB += fondo01_01.tileset->numTile;

    //PLANO A: Las 2 torres
    Estructuras = unpackMap(fondo01_01_torres.map, NULL);
    VDP_loadTileSet(fondo01_01_torres.tileset, id_tile_final_planB, CPU); //id_tile_final_planB->porque los tiles de las torres se guardan a continuacion
    VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);
    VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), 31, 4, 0, 0, 10, 11);
    id_tile_final_planA += fondo01_01.tileset->numTile;                  //id_tile_final_planA->indicara primera posicion libre mem VDP despues de cargar los tiles de las torres

    //Activa las interrupciones
    SYS_enableInts();

}

//CREA LA MIRA Y LA SITUA DENTRO DE LA PANTALLA VISIBLE
void crea_mira() {

    //valores iniciales
    Mira.x = 100;
    Mira.y = 100;
    Mira.movx = 2;
    Mira.movy = 2;

    //crea el sprite en el VDP
    Mira.spr_mira = SPR_addSprite(&sprite_mira, Mira.x, Mira.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
}


void maneja_mira(){

    u16 value = JOY_readJoypad(JOY_1);

    //si pulsamos izquierda...
    if(value & BUTTON_LEFT)
    {
        SPR_setPosition(Mira.spr_mira, Mira.x-=Mira.movx, Mira.y);
        Mira.x -= Mira.movx;
    }
    //si pulsamos derecha...
    if(value & BUTTON_RIGHT)
    {
        SPR_setPosition(Mira.spr_mira, Mira.x+=Mira.movx, Mira.y);
        Mira.x += Mira.movx;
    }
    //si pulsamos arriba...
    if(value & BUTTON_UP)
    {
        SPR_setPosition(Mira.spr_mira, Mira.x, Mira.y-=Mira.movy);
        Mira.y -= Mira.movy;
    }
    //si pulsamos abajo...
    if(value & BUTTON_DOWN)
    {
        SPR_setPosition(Mira.spr_mira, Mira.x, Mira.y+=Mira.movy);
        Mira.y += Mira.movy;
    }
    //'A' disparar
    if (value & BUTTON_A) maneja_bala_player();
}

//Deteccion para cambiar de ejemplo
void joyEvent(u16 joy, u16 changed, u16 state)
{
    if (state & (BUTTON_B))
    {
    //hemos pulsado el botón B (y no lo hemos soltado)
    }
    else if (changed & BUTTON_B) //hemos soltado el botón B
            {
               cerrojo=0;
            }

}




void crea_decorado()
{
        //Torre: inicializa vida
        Torre[0].vida  = 80;
        Torre[0].estado= 0;
        Torre[0].colX1 =   5;
        Torre[0].colY1 =  40;
        Torre[0].colX2 =  60;
        Torre[0].colY2 = 110;
        Torre[0].activa_impacto = 0;
        Torre[0].t_impacto = 0;

        Torre[1].vida  = 80;
        Torre[1].estado = 0;
        Torre[1].colX1 = 255;
        Torre[1].colY1 =  40;
        Torre[1].colX2 = 320;
        Torre[1].colY2 = 110;
        Torre[1].t_impacto = 0;

        cerrojo = 1; //para no salirnos del bucle
}


void maneja_decorado()
{
    //EJEMPLO 0: TINTADO CON PALETA DE ROJOS
    if(ejemplo==0)
    {
        if(Torre[0].activa_impacto!=0)
        {
            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1) //tinte rojo
            {
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);

            }else
            if(Torre[0].t_impacto>=T_TINTE) //tinte normal
            {
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);
            }
        }
    }


    //EJEMPLO 1: TINTADO LENTO CON PALETA DE ROJOS (QUE SE NOTE LA INFLUENCIA DE T_TINTE_LENTO)
    if(ejemplo==1)
    {
        if(Torre[0].activa_impacto!=0)
        {
            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1) //tinte rojo
            {
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL3, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);

            }else
            if(Torre[0].t_impacto>=T_TINTE_LENTO) //tinte LENTO, mas tiempo hasta volver al estado normal
            {
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);
            }
        }
    }


    //EJEMPLO 2: CAMBIANDO 1 SOLO COLOR
    if(ejemplo==2)
    {
        if(Torre[0].activa_impacto!=0)
        {
            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1) //tinte rojo
            {
                VDP_setPaletteColor(9,RGB24_TO_VDPCOLOR(0xff0000));  //(0x0098e5));

            }else
            if(Torre[0].t_impacto>=T_TINTE) //tinte normal
            {
                VDP_setPaletteColor(9,RGB24_TO_VDPCOLOR(0x4c260c));
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
            }
        }
    }


    //EJEMPLO 3: TINTADO USANDO UNA PALETA YA EXISTENTE
    if(ejemplo==3)
    {
        if(Torre[0].activa_impacto!=0)
        {
            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1)
            {
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);

            }else
            if(Torre[0].t_impacto>=T_TINTE)
            {
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);
            }
        }
    }


    //EJEMPLO 4:  BORRADO DEL PLANO COMPLETO
    if(ejemplo==4)
    {
        if(Torre[0].activa_impacto!=0)
        {
            //aqui se hace rapido

            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1)
            {
                //Borra plano A entero
                VDP_clearPlan(PLAN_A, TRUE);
                //Pinta la torre de la derecha
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), 31, 4, 0, 0, 10, 11);
                //vuelvo a escribir los mensajes en pantalla, se borran con VDP_clearPlan()
                muestraMENSAJES();
            }else
            if(Torre[0].t_impacto>=T_TINTE)
            {
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
                //repinta la torre de la izquierda
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 0, 0, 10, 11);
            }
        }
    }


    //EJEMPLO 5:  BORRADO PARCIAL
    //Se borra la parte del plano A de la torre de la izquierda que queda por ENCIMA
    //de la muralla. Es más rápida que la forma anterior y solo borra lo que nos interesa.
    if(ejemplo==5)
    {
        if(Torre[0].activa_impacto!=0)
        {
            //aqui se hace rapido

            Torre[0].t_impacto++;

            if(Torre[0].t_impacto==1)
            {
                //aqui lento
                VDP_clearTileMapRect(PLAN_A,0,3,10,5);  //borra parte del plano
            }else
            if(Torre[0].t_impacto>=T_TINTE)
            {
                Torre[0].activa_impacto = 0;
                Torre[0].t_impacto = 0;
                //repinta SOLO la parte del plano que borre antes
                VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB),-1, 3, 0, 0, 10, 5);
            }
        }
    }


    //EJEMPLO 6: TORRES DESTRUCTIBLES
    if(ejemplo==6)
    {
        if(Torre[0].vida<40 && Torre[0].estado == 0){
            Torre[0].estado = 1;
            Estructuras = unpackMap(fondo01_01_torres.map, NULL);
            VDP_loadTileSet(fondo01_01_torres.tileset, id_tile_final_planB, CPU);
            VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 10, 0, 10, 11);
            id_tile_final_planA += fondo01_01.tileset->numTile;
        }
        if(Torre[0].vida<20 && Torre[0].estado == 1) {
            Torre[0].estado = 2;
            VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), -1, 3, 20, 0, 10, 11);
            id_tile_final_planA += fondo01_01.tileset->numTile;
        }

        //Torre dcha
        if(Torre[1].vida<40 && Torre[1].estado == 0){
            Torre[1].estado = 1;
            Estructuras = unpackMap(fondo01_01_torres.map, NULL);
            VDP_loadTileSet(fondo01_01_torres.tileset, id_tile_final_planB, CPU);
            VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), 31, 4, 10, 0, 10, 11);
            id_tile_final_planA += fondo01_01.tileset->numTile;
        }
        if(Torre[1].vida<20 && Torre[1].estado == 1) {
            Torre[1].estado = 2;
            VDP_setMapEx(PLAN_A, Estructuras, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, id_tile_final_planB), 31, 4, 20, 0, 10, 11);
        }
    }

    //REINICIO
    if(ejemplo==7){
        SYS_hardReset();
    }

}

//SE CREA FUERA DE LA PANTALLA PARA QUE NO SEA VISIBLE
void crea_bala_player(){

    //Valores iniciales
    DisparoPlayer.esta_activo = 0;
    DisparoPlayer.x = FUERA_PANTALLA;
    DisparoPlayer.y = FUERA_PANTALLA;
    DisparoPlayer.spr_bala = SPR_addSprite(&sprite_bala_player, DisparoPlayer.x, DisparoPlayer.y,TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    //lo ponemos por encima de otros sprites ( SI LOS HUBIESE, que no es el caso pero viene bien saberlo)
    SPR_setDepth(DisparoPlayer.spr_bala,SPR_MIN_DEPTH);

}

//SI NO ESTABA ACTIVO el disparo, lo activo (bloqueando el acceso al IF)
// (es necesario porque por muy rápido que pulsemos, la MD registrará un montón de pulsaciones)
//Entonces SITUA el disparo en el centro de la mira
void maneja_bala_player() {

	if(DisparoPlayer.esta_activo == 0){

        DisparoPlayer.esta_activo = 1;

        DisparoPlayer.x = Mira.x + POS_MIRA;
        DisparoPlayer.y = Mira.y + POS_MIRA;
        SPR_setPosition(DisparoPlayer.spr_bala, DisparoPlayer.x, DisparoPlayer.y);
	}
}


//cambia la animacion manualmente, al terminar hace que el disparo desaparezca (lo ponemos fuera de la pantalla)
//(en vez de crear y destruir el sprite, no usamos mem dinamica), además desbloquea el disparo para poder volver a disparar
void trata_bala_player(){

    //cambia entre los frames de la animacion
    //ademas detecta_colisiones
    if(DisparoPlayer.esta_activo == 1 || DisparoPlayer.esta_activo == 2 ||
       DisparoPlayer.esta_activo == 3 || DisparoPlayer.esta_activo == 4)
    {
        SPR_nextFrame(DisparoPlayer.spr_bala);
        DisparoPlayer.esta_activo++;
        detecta_colision();
    }

    //al terminar la animacion DESAPARECE
    if(DisparoPlayer.esta_activo > 4 )
    {
        DisparoPlayer.x = FUERA_PANTALLA;
        DisparoPlayer.y = FUERA_PANTALLA;
        SPR_setPosition(DisparoPlayer.spr_bala, DisparoPlayer.x, DisparoPlayer.y);
        DisparoPlayer.esta_activo = 0;
    }
}


//PARA OPTIMIZAR solo se ejecuta este codigo durantes los 4 frames de animacion del disparo
//se compara posicion del disparo con la parte colisionable de cada torre
//en caso afirmativo, pierde vida y activamos el flag de colision de la torre
void detecta_colision(){

    for(s16 cont=0; cont<MAX_TORRES; cont++)
    {
        if(Torre[cont].vida>0)
        {
            if(DisparoPlayer.x > Torre[cont].colX1 && DisparoPlayer.x < Torre[cont].colX2 &&
               DisparoPlayer.y > Torre[cont].colY1 && DisparoPlayer.y < Torre[cont].colY2  )
            {
                    Torre[cont].vida--;
                    Torre[cont].activa_impacto++;
            }
        }
    }
}



//AYUDA EN PANTALLA
void muestraMENSAJES()
{
    VDP_setTextPalette(PAL1);   //con la PAL0 no se ve bien

    if(ejemplo == 0)
    {
    VDP_drawText("EJEMPLO 0: TINTADO CON PALERA DE ROJOS  ", 0, 20);
    VDP_drawText("     Cambio entre PAL0 y PAL3           ", 0, 21);
    VDP_drawText("  (dispara sobre la torre de la izq)    ", 0, 22);
    VDP_drawText("     Pulsa -A- para disparar            ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar           ", 0, 24);
    }

    if(ejemplo == 1)
    {
    VDP_drawText("EJEMPLO 1:tintado LENTO con PAL de ROJOS", 0, 20);
    VDP_drawText(" Cambio entre PAL0 y PAL3 pero tarda mas", 0, 21);
    VDP_drawText(" tiempo en volver a la paleta normal    ", 0, 22);
    VDP_drawText("                                        ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar           ", 0, 24);
    }

    if(ejemplo == 2)
    {
    VDP_drawText("EJEMPLO 2: TINTADO CON UN SOLO COLOR    ", 0, 20);
    VDP_drawText("                                        ", 0, 21);
    VDP_drawText(" Cambio del color 9 de la paleta.       ", 0, 22);
    VDP_drawText("                                        ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar           ", 0, 24);
    }

    if(ejemplo == 3)
    {
    VDP_drawText("EJEMPLO 3: TINTADO CON PAL EN USO       ", 0, 20);
    VDP_drawText("                                        ", 0, 21);
    VDP_drawText(" PAL0 y PAL1                            ", 0, 22);
    VDP_drawText("                                        ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar           ", 0, 24);
    }

    if(ejemplo == 4)
    {
    VDP_drawText("EJEMPLO 4: BORRADO COMPLETO DEL PLANO A  ", 0, 20);
    VDP_drawText("Se borra el plano A y se pinta la torre  ", 0, 21);
    VDP_drawText("de la derecha.                           ", 0, 22);
    VDP_drawText("                                         ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar            ", 0, 24);
    }

    if(ejemplo == 5)
    {
    VDP_drawText("EJEMPLO 5: BORRADO PARCIAL DEL PLANO A   ", 0, 20);
    VDP_drawText("Se borra solo la parte del plano A que   ", 0, 21);
    VDP_drawText("corresponde a la torre de la izq. por    ", 0, 22);
    VDP_drawText("encima de la muralla                     ", 0, 23);
    VDP_drawText("     Pulsa -B- para continuar            ", 0, 24);
    }

    if(ejemplo == 6)
    {
    VDP_drawText("EJEMPLO 6: TORRES DESTRUCTIBLES          ", 0, 20);
    VDP_drawText("                                         ", 0, 21);
    VDP_drawText(" Dispara sobre ambas torres para         ", 0, 22);
    VDP_drawText(" destruirlas. Repintado de Tiles         ", 0, 23);
    VDP_drawText("   Pulsa -B- para REINICIAR la ROM       ", 0, 24);
    }
}

//PINTA_ VIDA: Escribe en pantalla la vida de cada torre (solo la izq excepto al final)
void pinta_vida()
{
    //"%4d" justific a derechas, si no se pone al poner el contador a 0
    //los numeros nuevos (1 caracter:0,1,2...) se machacan/mezclan con
    //los viejos (que tienen hasta 3 caracteres:   999)

    char string01[32], string02[32];
    sprintf(string01, "%4d", Torre[0].vida);

    VDP_drawText("Torre[0].vida:",  1, 15);
    VDP_drawText(string01, 1, 16);
    if(ejemplo == 6)
    {
    sprintf(string02, "%4d", Torre[1].vida);
    VDP_drawText("Torre[1].vida:",  25, 15);
    VDP_drawText(string02, 32, 16);
    }
}
