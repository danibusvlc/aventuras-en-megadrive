/**
 *      @Title:  Leccion 02 - "controles (asincronos)"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

//La función asíncrona detecta cuando hay un cambio en el estado de algún botón
static void joyEvent(u16 joy, u16 changed, u16 state);


int main()
{
        VDP_drawText("Controles: FORMA ASINCRONA", 6, 4);

        //Antes del bucle principal
        JOY_setEventHandler(joyEvent);

        while(1){
            //NO HACE FALTA PONER NADA EN EL MAIN (al contrario que en la forma síncrona)

            VDP_waitVSync();
        }

       return (0);
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{

    //si pulsamos izquierda...
    if (state & BUTTON_LEFT)    VDP_drawText("IZQUIERDA", 8, 10);

    //si pulsamos derecha...
    if (state & BUTTON_RIGHT)   VDP_drawText("DERECHA", 20, 10);

    //si pulsamos izquierda...
    if (state & BUTTON_UP)      VDP_drawText("ARRIBA", 16, 8);

    //si pulsamos derecha...
    if (state & BUTTON_DOWN)    VDP_drawText("ABAJO", 16, 12);

    //si pulsamos A...
    if (state & BUTTON_A)       VDP_drawText("BOTON A", 6, 18);

    //si pulsamos B...
    if (state & BUTTON_B)       VDP_drawText("BOTON B", 14, 18);

    //si pulsamos C...
    if (state & BUTTON_C)       VDP_drawText("BOTON C", 22, 18);

    //si pulsamos X...
    if (state & BUTTON_X)       VDP_drawText("BOTON X", 6, 20);

    //si pulsamos Y...
    if (state & BUTTON_Y)       VDP_drawText("BOTON Y", 14, 20);

    //si pulsamos Z...
    if (state & BUTTON_Z)       VDP_drawText("BOTON Z", 22, 20);

    //si pulsamos START...
    if (state & BUTTON_START)   VDP_drawText("START", 10, 16);

    //si pulsamos MODE...
    if (state & BUTTON_MODE)    VDP_drawText("MODE", 20, 16);

   //si NO pulsamos nada, borra todo del título hacia abajo (VDP_clearTextArea() trabaja en tiles)
    if ( ((!(state & BUTTON_LEFT)) && (changed & BUTTON_LEFT))   ||
         ((!(state & BUTTON_RIGHT))&& (changed & BUTTON_RIGHT))  ||
         ((!(state & BUTTON_UP))   && (changed & BUTTON_UP))     ||
         ((!(state & BUTTON_DOWN)) && (changed & BUTTON_DOWN) )  ||
         ((!(state & BUTTON_A))    && (changed & BUTTON_A))      ||
         ((!(state & BUTTON_B))    && (changed & BUTTON_B))      ||
         ((!(state & BUTTON_C))    && (changed & BUTTON_C))      ||
         ((!(state & BUTTON_X))    && (changed & BUTTON_X))      ||
         ((!(state & BUTTON_Y))    && (changed & BUTTON_Y))      ||
         ((!(state & BUTTON_Z))    && (changed & BUTTON_Z))      ||
         ((!(state & BUTTON_START))&& (changed & BUTTON_START))  ||
         ((!(state & BUTTON_MODE)) && (changed & BUTTON_MODE)))

        VDP_clearTextArea(0,7,40,28);
}
