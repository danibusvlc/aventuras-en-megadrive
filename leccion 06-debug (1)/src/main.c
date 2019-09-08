/**
 *      @Title:  Leccion 06 - "Debug"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include "KDebug.h"  // <<--- PARA DEBUGGEAR general
#include "tools.h"   // <<--- PARA DEBUGGEAR funciones klog
#include "timer.h"   // <<--- PARA DEBUGGEAR timers


//declaracion funciones
void reset();


int main()
{


    while(1)
    {
        //variable para guardar la entrada del mando
        u16 value;





        //Ejemplo 1
        //Pausa el juego, hasta pulsar ESC

        VDP_drawText("Leccion 6: DEBUG", 1, 1);
        VDP_drawText("Ejemplo 1: KDebug_Halt()               ", 1, 3);
        VDP_drawText("                                       ", 1, 4);
        VDP_drawText("   Pulsa ESC para seguir               ", 1, 5);
        VDP_drawText("                                       ", 1, 6);

        KDebug_Halt();

        VDP_drawText("                                       ", 1, 4);
        VDP_drawText("   Has pulsado ESC!                    ", 1, 5);
        VDP_drawText("                                       ", 1, 6);

        VDP_drawText("Pulsa -A- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }






        //Ejemplo 2
        //Muestra un mensaje en la ventana Messages de GensKMod
        //Utilizamos primero cadenas de texto "normales", después
        //usamos strings, luego convertimos números a cadenas y
        //finalmente todo a la vez

        VDP_drawText("Leccion 6: DEBUG", 1, 1);
        VDP_drawText("Ejemplo 2: KDebug_Alert()              ", 1, 3);
        VDP_drawText("Muestra un mensaje en ventana de debug:", 1, 5);
        VDP_drawText("  GensKMod->CPU/Debug/Messages.        ", 1, 6);
        VDP_drawText("                                       ", 1, 7);
        VDP_drawText("Ejemplos:                              ", 1, 8);
        VDP_drawText(" * Texto escrito y texto en variable   ", 1, 9);
        VDP_drawText(" * Lineas en blanco                    ", 1, 10);
        VDP_drawText(" * Num enteros positivos y negativos   ", 1, 11);
        VDP_drawText(" * Num en punto fijo positivos y negati", 1, 12);
        VDP_drawText(" * Combinacion de todo lo anterior     ", 1, 13);


        KDebug_Alert("Hola soy un mensaje corto");
        KDebug_Alert("Hola soy un mensaje excesivamente largo para caber en una sola linea y por tanto aparezco en dos");
        KDebug_Alert(" "); //<- ESTA ES UNA LINEA EN BLANCO
        KDebug_Alert(" arriba y abajo una linea en blanco");
        KDebug_Alert(" "); //<- ESTA ES UNA LINEA EN BLANCO


        char *char_entrada = "esto es una cadena de ejemplo";
        char char_salida[32];

        //imprime "esto es una cadena de ejemplo";
        KDebug_Alert(char_entrada);

        //imprime un número entero positivo,antes debemos convertirlo a texto
        int mi_numero = 1200;
        intToStr(mi_numero,char_salida,1);
        KDebug_Alert(char_salida);

        //imprime un número entero negativo
        mi_numero = -1200;
        intToStr(mi_numero,char_salida,1);
        KDebug_Alert(char_salida);

        //imprime un número en punto fijo positivo
        fix16 mi_numero16 = FIX16(5.4);
        fix16ToStr(mi_numero16,char_salida,1);
        KDebug_Alert(char_salida);

        //imprime un número en punto fijo negativo
        mi_numero16 = FIX16(-5.4);
        fix16ToStr(mi_numero16,char_salida,1);
        KDebug_Alert(char_salida);

        //vamos a imprimir todo combinado (cadenas y un entero)
        char *cadena_A = " ... mas seis son ... ";
        char cadena_B[50], cadena_C[50];

        strcpy(cadena_B, "Cuatro...");
        strcat(cadena_B, cadena_A);
        mi_numero = 10;
        sprintf(cadena_C, "%s %d", cadena_B, mi_numero);
        KDebug_Alert(cadena_C);


        VDP_drawText("Pulsa -B- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }












        //Ejemplo 3
        //Muestra el valor de una variable en la ventana Messages de GensKMod
        //En este caso probamos con INTs. En la ventana el valor se muestra
        //siempre en hexadecimal, por tanto tenerlo en cuenta a la chequear el
        //valor.

        VDP_drawText("Leccion 6: DEBUG                       ", 1, 1);
        VDP_drawText("Ejemplo 3: KDebug_AlertNumber()        ", 1, 3);
        VDP_drawText("Probamos a mostrar una variable entera ", 1, 5);
        VDP_drawText("                                       ", 1, 6);
        VDP_drawText("int mi_variable =  100                 ", 1, 7);
        VDP_drawText("int mi_variable = -100                 ", 1, 8);
        VDP_drawText("                                       ", 1, 9);
        VDP_drawText("KDebug_AlertNumber muestra valor       ", 1, 10);
        VDP_drawText("64 y FFFFFF9C respectivamente          ", 1, 11);
        VDP_drawText("coincide con sus valores hexadecimales ", 1, 12);
        VDP_drawText("                                       ", 1, 13);
        VDP_drawText(" No es precisamente intuitivo          ", 1, 14);

        KDebug_Alert("--- INT entero ---");
        int mi_variable = 100;
        KDebug_AlertNumber(mi_variable);

        KDebug_Alert("--- INT entero negativo ---");
        mi_variable = -100;
        KDebug_AlertNumber(mi_variable);

        VDP_drawText("Pulsa -C- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }







        //Ejemplo 4
        //Funciones KLOG
        // void KLog(char* text);
        // void KLog_U1(char* t1, u32 v1);
        // void KLog_U2(char* t1, u32 v1, char* t2, u32 v2);
        // void KLog_S1(char* t1, s32 v1);
        // void KLog_S2(char* t1, s32 v1, char* t2, s32 v2);
        // etc

        VDP_drawText("Leccion 6: DEBUG                       ", 1, 1);
        VDP_drawText("Ejemplo 4: Funciones KLOG()            ", 1, 3);
        VDP_drawText("               donde t=texto, v=entero ", 1, 4);
        VDP_drawText(" TEXTO:  KLog(t)                       ", 1, 5);
        VDP_drawText("                                       ", 1, 6);
        VDP_drawText(" ENTERO POSITIVO: KLog_U1(t1,v1)       ", 1, 7);
        VDP_drawText("                                       ", 1, 8);
        VDP_drawText(" (2) \"       \": KLog_U1(t1,v1,t2,v2) ", 1, 9);
        VDP_drawText("                                       ", 1, 10);
        VDP_drawText(" ENTERO POSIT y NEGAT: KLog_S1(t1,v1)  ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" (2) \"   \"   \": KLog_S1(t1,v1,t2,v2)", 1, 13);
        VDP_drawText("                                       ", 1, 14);


        KLog("--- KLog(t) ---");
        char *texto = "esto es otra cadena de ejemplo";
        KLog(texto);

        KLog("--- KLog_U1 (ENTERO POSITIVO) ---");
        int mi_variable_u1 = 100;
        KLog_U1("entero positivo:", mi_variable_u1);

        KLog("--- KLog_U2 (2 ENTEROS POSITIVOS) ---");
        int mi_variable_u2 = 56;
        KLog_U2("primer entero:", mi_variable_u1," ,segundo entero:",mi_variable_u2);

        KLog("--- KLog_S1 (ENTERO POSITIVOS+NEGATIVO) ---");
        mi_variable_u1  = +100;
        KLog_S1("entero positivo:", mi_variable_u1);

        KLog("--- KLog_S1 (ENTERO POSITIVOS+NEGATIVO) ---");
        mi_variable_u1  = +100;
        mi_variable_u2 = -56;
        KLog_S2("entero positivo:", mi_variable_u1," ,entero negativo:",mi_variable_u2);



        VDP_drawText("Pulsa -A- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }












        //Ejemplo 5
        //Ejemplo de uso del timer del SGDK (I)
        //

        VDP_drawText("Leccion 6: DEBUG                       ", 1, 1);
        VDP_drawText("Ejemplo 5: TIMERs                      ", 1, 3);
        VDP_drawText("                                       ", 1, 4);
        VDP_drawText(" Pulsa B para iniciar el timer         ", 1, 5);
        VDP_drawText("                                       ", 1, 6);
        VDP_drawText("        -------                        ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText(" ######|   |-- |#######                ", 1, 9);
        VDP_drawText(" ######|   |   |#######                ", 1, 10);
        VDP_drawText("        -------                        ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" KDebug_StartTimer();                  ", 1, 13);
        VDP_drawText(" precision: 7.610.000 ciclos/seg       ", 1, 14);

        VDP_drawText("Pulsa -B- para INICIAR timer           ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }

        //inicia el timer
        KLog("--- INICIA EL TIMER!! ---");
        KDebug_StartTimer();

        VDP_drawText(" Pulsa C para parar el timer           ", 1, 5);
        VDP_drawText("Pulsa -C- para PARAR timer             ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }

        //para el timer
        KLog("--- PARA EL TIMER!! ---");
        KDebug_StopTimer();


        VDP_drawText(" Consulta el tiempo en la ventana Debug", 1, 5);
        VDP_drawText(" Divide resultad por 7.610.000=segundos", 1, 6);
        VDP_drawText("        -------                        ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText(" ######| - 0 - |#######                ", 1, 9);
        VDP_drawText(" ######|   |   |#######                ", 1, 10);
        VDP_drawText("        -------                        ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" KDebug_StopTimer();                   ", 1, 13);
        VDP_drawText("                                       ", 1, 14);


        VDP_drawText("Pulsa -A- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }



        //Ejemplo 6
        //Ejemplo de uso del timer del SGDK (II)
        // u32 getSubTick(); Returns elapsed subticks from console reset.

        VDP_drawText("Leccion 6: DEBUG                       ", 1, 1);
        VDP_drawText("Ejemplo 6: TIMERs (II)                 ", 1, 3);
        VDP_drawText("                                       ", 1, 4);
        VDP_drawText(" Existen otras tipos de timers que     ", 1, 5);
        VDP_drawText(" podemos usar.                         ", 1, 6);
        VDP_drawText("        -------                        ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText(" ######|   |-- |#######                ", 1, 9);
        VDP_drawText(" ######|   |   |#######                ", 1, 10);
        VDP_drawText("        -------                        ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" getSubTick();                         ", 1, 13);
        VDP_drawText(" precision: 76800 subticks/seg         ", 1, 14);

        VDP_drawText("Pulsa -B- para INICIAR timer           ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }


        //inicia el timer
        KLog("--- INICIA EL TIMER!! ---");
        KLog("Timer started");
        u32 starttime = getSubTick();

        VDP_drawText(" Pulsa C para parar el timer           ", 1, 5);
        VDP_drawText("Pulsa -C- para PARAR timer             ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }


        VDP_drawText(" A diferencia de la funcion anterior,  ", 1, 5);
        VDP_drawText(" podemos trabajar con el resultado.    ", 1, 6);
        VDP_drawText("        -------                        ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText(" ######| - 0 - |#######                ", 1, 9);
        VDP_drawText(" ######|   |   |#######                ", 1, 10);
        VDP_drawText("        -------                        ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" getSubTick();                         ", 1, 13);
        VDP_drawText(" Tiempo:                   subticks    ", 1, 14);
        VDP_drawText("                           segundos    ", 1, 15);

        //para el timer y sacamos la diferencia
        KLog("--- PARA EL TIMER!! ---");
        u32 endtime = getSubTick();
        //sacamos resultado por pantalla y se lo mostramos al usuario
        char outputString[16];
        uintToStr(endtime - starttime, outputString, 16);                       //subticks
        VDP_drawText(outputString, 10, 14);
        uintToStr((endtime - starttime)/SUBTICKPERSECOND, outputString, 16);    //segundos
        VDP_drawText(outputString, 10, 15);

        //sacamos resultado por pantalla Debug
        KLog_U1("subticks:   ",(endtime - starttime));                      // en subticks
        KLog_U1("segundos: ",((endtime - starttime)/SUBTICKPERSECOND));     // en segundos


        VDP_drawText("Pulsa -A- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }





        //Ejemplo 7
        //Ejemplo de uso del timer del SGDK (III)
        //Utilizamos los timers de tools.h

        VDP_drawText("Leccion 6: DEBUG                       ", 1, 1);
        VDP_drawText("Ejemplo 7: TIMERs (III)                ", 1, 3);
        VDP_drawText("                                       ", 1, 4);
        VDP_drawText(" Ahora veremos startTimer();           ", 1, 5);
        VDP_drawText(" Hasta 16 timers disponibles (0-15)    ", 1, 6);
        VDP_drawText("        -------                        ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText(" ######|   |-- |#######                ", 1, 9);
        VDP_drawText(" ######|   |   |#######                ", 1, 10);
        VDP_drawText("        -------                        ", 1, 11);
        VDP_drawText("                                       ", 1, 12);
        VDP_drawText(" Al pulsar -B- se iniciaran 2 timers   ", 1, 13);
        VDP_drawText(" el primero se detendra al pulsar -C-  ", 1, 14);
        VDP_drawText(" el segundo al pulsar -A-              ", 1, 15);

        VDP_drawText("Pulsa -B- para INICIAR ambos timers    ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }


        //inicia el timer
        KLog("--- INICIA LOS TIMERS!! ---");
        startTimer(0);          //inicia el timer 0
        startTimer(1);          //inicia el timer 1

        VDP_drawText(" Pulsa C para parar el timer 0         ", 1, 5);
        VDP_drawText(" Pulsa -C- para PARAR timer 0          ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_C) break; }

        //para el timer
        KLog("--- PARA EL TIMER 0 ---");
        //recoge el tiempo del timer 0 y detiene el temporizador (FALSE)
        u32 tiempo_timer0 = getTimer(0, TRUE);


        VDP_drawText(" Pulsa A para parar el timer 1         ", 1, 5);
        VDP_drawText(" Pulsa -A- para PARAR timer 1          ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_A) break; }

        //para el timer
        KLog("--- PARA EL TIMER 1 ---");
        //recoge el tiempo del timer 1 y detiene el temporizador (FALSE)
        u32 tiempo_timer1 = getTimer(1, TRUE);


        VDP_drawText("        -------                        ", 1, 5);
        VDP_drawText(" ######|   |   |#######                ", 1, 6);
        VDP_drawText(" ######| - 0 - |#######                ", 1, 7);
        VDP_drawText(" ######|   |   |#######                ", 1, 8);
        VDP_drawText("        -------                        ", 1, 9);
        VDP_drawText("                                       ", 1, 10);
        VDP_drawText(" Timer 0                    subticks   ", 1, 11);
        VDP_drawText("                            segundos   ", 1, 12);
        VDP_drawText("                                       ", 1, 13);
        VDP_drawText(" Timer 1                    subticks   ", 1, 14);
        VDP_drawText("                            segundos   ", 1, 15);


        //timer 0
        //sacamos resultado por pantalla y se lo mostramos al usuario
        uintToStr(tiempo_timer0, outputString, 16);                       //subticks
        VDP_drawText(outputString, 10, 11);
        uintToStr(tiempo_timer0/SUBTICKPERSECOND, outputString, 16);      //segundos
        VDP_drawText(outputString, 10, 12);

        //sacamos resultado por pantalla Debug
        KLog_U1("subticks:   ",tiempo_timer0);                      // en subticks
        KLog_U1("segundos: ",(tiempo_timer0/SUBTICKPERSECOND));     // en segundos


        //timer 1
        //sacamos resultado por pantalla y se lo mostramos al usuario
        uintToStr(tiempo_timer1, outputString, 16);                       //subticks
        VDP_drawText(outputString, 10, 14);
        uintToStr(tiempo_timer1/SUBTICKPERSECOND, outputString, 16);      //segundos
        VDP_drawText(outputString, 10, 15);

        //sacamos resultado por pantalla Debug
        KLog_U1("subticks:   ",tiempo_timer1);                      // en subticks
        KLog_U1("segundos: ",(tiempo_timer1/SUBTICKPERSECOND));     // en segundos

        VDP_drawText("Pulsa -B- para continuar               ", 1, 20);
        while(1){ value = JOY_readJoypad(JOY_1); if (value & BUTTON_B) break; }








        //hacemos un reset
        reset();

        VDP_waitVSync();
    }

return 0;
}




// soft reset
void reset()
{
    asm("move   #0x2700,%sr\n\t"
        "move.l (0),%a7\n\t"
        "move.l (4),%a0\n\t"
        "jmp    (%a0)");
}
