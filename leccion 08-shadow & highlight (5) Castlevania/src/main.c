/**
 *      @Title:  Leccion 08 - "Shadow & highlight (5) Castlevania"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>

#include "fondos.h"   //carga los fondos
#include "sprites.h"  //carga los sprites
#include "tools.h"    //debug
#include "joy.h"      //mando

//animaciones Morris
#define ANIM_STAND      0
#define ANIM_RUN        1

//animaciones Muerte
#define ANIM_MUERTE_NORMAL      0
#define ANIM_MUERTE_COLOR13     1
#define ANIM_MUERTE_COLOR14     2
#define ANIM_MUERTE_COLOR15     3

//// CONSTANTES PARA EL MAPA DE PRIORIDADES/TILES ////////////

        //El mapa de prioridad son 2 pantallas = son 80×28 tiles
        #define SCREEN_WIDTH_TILES     80
        #define SCREEEN_HEIGHT_TILES   28

        #define SCENARIO_POS_X          0
        #define SCENARIO_POS_Y          0

        #define SCENARIO_WIDTH_TILES    SCREEN_WIDTH_TILES
        #define SCENARIO_HEIGHT_TILES   SCREEEN_HEIGHT_TILES
        //Total tiles de la pantalla = 80x28 = 2240 tiles
        #define SCENARIO_NUM_TILES      SCENARIO_WIDTH_TILES * SCENARIO_HEIGHT_TILES


//// CONSTANTES Y METODOS PARA DIAGONALES ////////////

        // Some data to deal with graphical data
        #define NUM_COLUMNS     40
        #define NUM_ROWS        28
        #define NUM_LINES       NUM_ROWS * 8
        //Pone a 0 todos los elementos de line_scroll_data[]
        #define InitializeScrollTable(); \
            for(i = 0; i < NUM_LINES; i++) line_scroll_data[i] = FIX16(0);
        //Pone la sig secuencia (0,1,2,3...223) en line_speed_data[]
        #define InitializeSpeedTable(); \
            line_speed_data[0] = FIX16(0); \
            for(i = 1; i < NUM_LINES; i++) \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], FIX16(1));
        //idem pero con diagonal dinamica
        #define InitializeSpeedTable_dinamica(); \
            line_speed_data[0] = FIX16(0); \
            for(i = 1; i < NUM_LINES; i++){ \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], FIX16(1)); \
               line_speed_data[i] = fix16Add(line_speed_data[i-1], diagonal_dinamica); }
        //para la diagonal dinamica
        fix16 diagonal_dinamica=FIX16(1);


//Declaramos la funcion para recoger la entrada del mando
static void handleInput();

//Decl. Funciones
static void mueveEnemigos();
static void muestraMENSAJES();
void myJoyHandler( u16 joy, u16 changed, u16 state);

//Sprite de John Morris
Sprite* spr_Morris;

// Posicion en pantalla de Morris
int morris_x = 64;
int morris_y = 152;

//Sprite de la Muerte
Sprite* spr_Muerte;

//Posicion en pantalla de la Muerte
int muerte_x = 220;
int muerte_y = 50;
//int enemigoSentidoMovimiento; //OPCIONAL: ver mueveEnemigos()

//Para el mov senoidal de la Muerte
#define nivelMedioMuerteY 70
int velocidadmovx = 1;
int sinSpeed = 1;
int Amplitude = 1;

//Para los estados
int estado = 0;

//variable para llevar el control de tiles
u16 ind;



int main()
{

    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //recoge las paletas de los fondos y los asigna a la primera y segunda paleta del sistema
    VDP_setPalette(PAL0,fondo1.palette->data);
    VDP_setPalette(PAL1,fondo2.palette->data);

    //carga los fondos en el VDP
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_B, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &fondo2, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo2.tileset->numTile;

    //recoge la paleta de Morris
    VDP_setPalette(PAL2,morris_sprite.palette->data);

    //añade el sprite de Morris
    spr_Morris = SPR_addSprite(&morris_sprite, morris_x, morris_y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

    //añade los enemigos

    //OPCIONAL: ver mueveEnemigos()
    //enemigoSentidoMovimiento =  1;

    //recoge la paleta de los enemigos y la mete en la 4a paleta del sistema
    VDP_setPalette(PAL3,muerte_sprite.palette->data);

    //sprite muerte
    spr_Muerte = SPR_addSprite(&muerte_sprite, muerte_x, muerte_y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));

    //animacion inicial
    SPR_setAnim(spr_Muerte, ANIM_MUERTE_NORMAL);


    //deteccion mando (asincrono)
	JOY_init();
	JOY_setEventHandler( &myJoyHandler );

	//El texto se pintara con la PAL3 (siempre se usa el color 15, recordar que empieza en 0)
    VDP_setTextPalette(PAL3);

    SPR_update(); // <- no es necesaria pero recomendable


    //Bucle principal
    while(TRUE)
    {
        //recoje la entrada de los mandos (sincrono)
        handleInput();

        //mueve los enemigos
        mueveEnemigos();

        //muestra Mensajes al usuario
        muestraMENSAJES();

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }

    return 0;
}




// MANDO DETECCION SINCRONA
//Recoje la entrada del mando (izq,dcha) y actualiza la posicion del
//sprite spr_Morris. El motivo de hacer esto aqui y NO en la parte
//asincrona es que la funcion asincrona detecta cambios y si dejamos
//pulsado p.e. derecha, la funcion asincrona solo detectaria la 1a pulsacion
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
    {
        SPR_setPosition(spr_Morris, morris_x--, morris_y);
        SPR_setHFlip(spr_Morris, TRUE);
        SPR_setAnim(spr_Morris, ANIM_RUN);
    }

    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
    {
        SPR_setPosition(spr_Morris, morris_x++, morris_y);
        SPR_setHFlip(spr_Morris, FALSE);
        SPR_setAnim(spr_Morris, ANIM_RUN);
    }

    //si no pulsamos
    if ((!(value & BUTTON_RIGHT)) && (!(value & BUTTON_LEFT)))
    {
        SPR_setAnim(spr_Morris, ANIM_STAND);
    }



    //ESTADO 12: DIAGONALES DINAMICAS
    if(estado == 12)
    {
        //cambio diagonales
        if(value & BUTTON_UP)
        {
            //incrementamos la diagonal
            diagonal_dinamica = fix16Add(diagonal_dinamica,FIX16(0.025));
            //Contador
            u16 i;
            //como antes
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];
            //en este caso la tabla cambia
            InitializeScrollTable();
            InitializeSpeedTable_dinamica();
            VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

                for(i = 0; i < NUM_LINES; i++)
                {
                    line_scroll_data[i] = fix16Add(line_scroll_data[i], line_speed_data[i]);
                    aux[i] = fix16ToInt(line_scroll_data[i]);
                }

                VDP_setHorizontalScrollLine(PLAN_A, 0, aux, NUM_LINES, 1);
        }
        //cambio diagonales
        if(value & BUTTON_DOWN)
        {
            //incrementamos la diagonal
            diagonal_dinamica = fix16Sub(diagonal_dinamica,FIX16(0.025));
            //Contador
            u16 i;
            //como antes
            fix16 line_scroll_data[NUM_LINES];
            fix16 line_speed_data[NUM_LINES];
            s16 aux[NUM_LINES];
            //en este caso la tabla cambia
            InitializeScrollTable();
            InitializeSpeedTable_dinamica();
            VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

                for(i = 0; i < NUM_LINES; i++)
                {
                    line_scroll_data[i] = fix16Add(line_scroll_data[i], line_speed_data[i]);
                    aux[i] = fix16ToInt(line_scroll_data[i]);
                }

                VDP_setHorizontalScrollLine(PLAN_A, 0, aux, NUM_LINES, 1);
        }
    }

}


// MANDO DETECCION A-SINCRONA
// Con A, B, C vamos jugando con las distintas formas de usar S/H
void myJoyHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
	    //ESTADO 0 : partimos de S/H DESACTIVADO
	    //todo 'aparece normalmente'

	    //ESTADO 1: ACTIVA S&H
	    //todo tiene NO prioridad, por tanto todo se oscurece SHADOW
        if(estado == 0 && (changed & BUTTON_A) )
        {
            estado = 1;
            VDP_setHilightShadow(1);
        }

	    //ESTADO 2: S&H ACTIVADO
	    // ACTIVA PRIORIDAD en el sprite del player
        if(estado == 1 && (changed & BUTTON_B) )
        {
            estado = 2;
            SPR_setPriorityAttribut(spr_Morris, TRUE);
        }

	    //ESTADO 3: S&H ACTIVADO
        // ACTIVA PRIORIDAD en la muerte
        if(estado == 2 && (changed & BUTTON_C) )
        {
            estado = 3;
            SPR_setPriorityAttribut(spr_Muerte, TRUE);
        }

	    //ESTADO 4: S&H ACTIVADO
        // Dibuja SOLO en una PARTE del FONDO (plano B) a la que daremos PRIORIDAD
        // Para ello usamos una imagen que contiene SOLO la luna, y la pintamos directamente
        // en la posición que debe ocupar en pantalla (no pintamos la pantalla entera)
        // De esta forma le damos prioridad solo a esos tiles y NO a toda la pantalla entera
        if(estado == 3 && (changed & BUTTON_A) )
        {
            estado = 4;
            VDP_drawImageEx(PLAN_B, &fondo3, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 14, 4, FALSE, TRUE);
            ind += fondo3.tileset->numTile;
        }

	    //ESTADO 5: S&H ACTIVADO
        // Todo con prioridad
        if(estado == 4 && (changed & BUTTON_B) )
        {
            estado = 5;
            //carga tiles desde el principio de la VRAM para no malgastarla
            ind = TILE_USERINDEX;
            VDP_drawImageEx(PLAN_B, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo1.tileset->numTile;
            VDP_drawImageEx(PLAN_A, &fondo2, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo2.tileset->numTile;
        }

 	    //ESTADO 6: S&H DESACTIVADO
        // Todo con prioridad
        if(estado == 5 && (changed & BUTTON_C) )
        {
            estado = 6;
            //los fondos SIN prioridad
            ind = TILE_USERINDEX;
            VDP_drawImageEx(PLAN_B, &fondo1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo1.tileset->numTile;
            VDP_drawImageEx(PLAN_A, &fondo2, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo2.tileset->numTile;
            //El player SIN prioridad
            SPR_setPriorityAttribut(spr_Morris, FALSE);
            //Cambio de animacion
            SPR_setAnim(spr_Muerte, ANIM_MUERTE_COLOR13);
            //del color 0 al color 13 saldra sombreado si tiene prioridad baja o normal si tiene prioridad alta
            SPR_setPriorityAttribut(spr_Morris, FALSE);

        }

 	    //ESTADO 7: S&H DESACTIVADO
        // Todo con prioridad
        if(estado == 6 && (changed & BUTTON_A) )
        {
            estado = 7;
            //del color 0 al color 13 saldra sombreado si tiene prioridad baja o normal si tiene prioridad alta
            SPR_setAnim(spr_Muerte, ANIM_MUERTE_COLOR13);
            SPR_setPriorityAttribut(spr_Morris, TRUE);
        }

	    //ESTADO 8: S&H ACTIVADO
        // solo explicacion
        if(estado == 7 && (changed & BUTTON_B) )
        {
            estado = 8;
            //con el color 14
            SPR_setAnim(spr_Muerte, ANIM_MUERTE_COLOR14);
        }

	    //ESTADO 9: S&H ACTIVADO
        // solo explicacion
        if(estado == 8 && (changed & BUTTON_C) )
        {
            estado = 9;
            //con el color 15
            SPR_setAnim(spr_Muerte, ANIM_MUERTE_COLOR15);
        }

	    //ESTADO 10: RESETEO, nuevo fondo y ejemplo mapa prioridad
        // solo explicacion
        if(estado == 9 && (changed & BUTTON_A) )
        {
            estado = 10;

            SPR_reset();                 //adios sprites
            VDP_clearPlan(PLAN_A, TRUE); //adios fondo. TRUE espera a que termine
            VDP_clearPlan(PLAN_B, TRUE); //de borrarlo

            //"Empezamos desde cero"
            //pone la pantalla a 320x224
            VDP_setScreenWidth320();
            //inicializa motor de sprites
            SPR_init(0, 0, 0);
            //recoge la paleta de Morris
            VDP_setPalette(PAL2,morris_sprite.palette->data);
            //añade el sprite de Morris
            spr_Morris = SPR_addSprite(&morris_sprite, morris_x, morris_y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
            //sprite muerte
            spr_Muerte = SPR_addSprite(&muerte_sprite, muerte_x, muerte_y, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
            //animacion inicial
            SPR_setAnim(spr_Muerte, ANIM_MUERTE_NORMAL);
            //Posicion en pantalla de Morris y la Muerte (quiza el usuario los dejo fuera d la pantalla)
            morris_x = 64;
            morris_y = 152;
            muerte_x = 220;
            muerte_y = 50;

            // DATOS _________________________________________
            //crea una vector con tantas posiciones como tiles tiene la pantalla (1120)
            u16 tilemap_buffer[SCENARIO_NUM_TILES];
            //puntero a la primera posición del vector
            u16 *aplan_tilemap = &tilemap_buffer[0];
            //inicializa el vector todo a cero (por defecto hay 'basura')
            for(int j=0; j<SCENARIO_NUM_TILES;j++) tilemap_buffer[j]=0;

            //puntero A LOS TILES de la imagen "mapa de prioridades", ojo, a sus tiles no a una imagen
            u16 *shadowmap_tilemap = fondo_doble_prioridad.map->tilemap;
            //para contar, valor inicial tiles totales de una pantalla
            u16 numtiles = SCENARIO_NUM_TILES;

            // PROCESO ____________________________________________

            // BUCLE: Recorremos desde el ultimo al primer tile, para cada uno comprobamos el valor de ese tile
            // * si es 0 (color negro, indice 0 de paleta), no hacemos nada
            // * si NO es cero (cualquier otro color, indice distinto 0 de paleta) marcamos ese tile
            // con prioridad (TILE_ATTR_PRIORITY_MASK)

            while(numtiles--)
            {
                //comprueba el valor del color del tile (0..15)
                if(*shadowmap_tilemap)
                    *aplan_tilemap |= TILE_ATTR_PRIORITY_MASK;
                    //equivale a *aplan_tilemap = *aplan_tilemap | TILE_ATTR_PRIORITY_MASK;
                    //TILE_ATTR_PRIORITY_MASK = 0x8000h. Al vector aplan_tilemap (fue inicilizado a CERO)
                    //le sumamos '1' cuando el valor del color del tile es distinto de cero

                aplan_tilemap++;        //avanza en el vector
                shadowmap_tilemap++;    //avanza en el otro vector
            }

            //EN ESTE PUNTO tenemos el vector del mapa de prioridades, en cada posicion tiles configuradas con la prioridad

            //recoge la paleta del fondo
            VDP_setPalette(PAL0,fondo_doble.palette->data);

            //carga los fondos en el VDP

            // Carga el planoB (el escenario "real" tal cual es, nada raro)
            ind = TILE_USERINDEX;
            VDP_drawImageEx(PLAN_B, &fondo_doble, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
            ind += fondo_doble.tileset->numTile;

            // Carga en el plano A en posicion (0,0), 40x28 tiles con el contenido que hay en &tilemap_buffer[0]
            VDP_setTileMapDataRectEx(PLAN_A, &tilemap_buffer[0], 0/*u16 basetile*/,
                SCENARIO_POS_X, SCENARIO_POS_Y,
                SCENARIO_WIDTH_TILES, SCENARIO_HEIGHT_TILES, SCENARIO_WIDTH_TILES);
            ind += fondo_doble_prioridad.tileset->numTile;

            // Enable HL/S mode
            VDP_setHilightShadow(1);

        }


	    //ESTADO 11: DIAGONALES
        if(estado == 10 && (changed & BUTTON_B) )
        {
            estado = 11;

            //Contador
            u16 i;
            //Crea dos vectores ("Line scroll buffers") y una variable
            fix16 line_scroll_data[NUM_LINES];  // Current line scroll values
            fix16 line_speed_data[NUM_LINES];   // Line scroll speeds
            s16 aux[NUM_LINES];                 // Needed for VDP_setHorizontalScrollLine
            // Inicializa los vectores "scroll buffers"
            InitializeScrollTable();
            InitializeSpeedTable();
            // Screen setting
            VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_PLANE);

                for(i = 0; i < NUM_LINES; i++)
                {
                    // Sum the speed value
                    line_scroll_data[i] = fix16Add(line_scroll_data[i], line_speed_data[i]);

                    // An auxiliar "regular integer" buffer is needed for VDP_setHorizontalScrollLine
                    aux[i] = fix16ToInt(line_scroll_data[i]);

                }// end for(NUM_LINES)

            // Este comando le dice cuanto tiene que desplazar el scroll del plano A (PLAN_A),
            // para cada línea, empezando desde la cero (0) con un offset determinado (aux).
            // El numero de lineas es (NUM_LINES) en este caso toda la pantalla, y (1) es
            // cómo hacerlo, en este caso usando la CPU, y por tanto esto se hará durante el
            // período donde no se dibuja en pantalla
            VDP_setHorizontalScrollLine(PLAN_A, 0, aux, NUM_LINES, 1);
                //al desplazar TODO el plano A, el texto también sale "en diagonal"

        }


 	    //ESTADO 12: DIAGONALES DINAMICAS
        if(estado == 11 && (changed & BUTTON_C) )
        {
            estado = 12;
        }

	}

}


static void muestraMENSAJES()
{
    if(estado == 0) //estado inicial al empezar el programa
    {
    VDP_drawText("S&H: OFF                               ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE: OFF", 1, 2);
    VDP_drawText("-A- Activa S&H                         ", 1, 26);
    VDP_drawText("                                       ", 1, 27);
    }

    if(estado == 1)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE: OFF", 1, 2);
    VDP_drawText("-B- Activa PRIORIDAD en Player         ", 1, 26);
    VDP_drawText("En S/H:  Prioridad=delante=color normal", 1, 27);
    }

    if(estado == 2)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS:  ON, MUERTE: OFF", 1, 2);
    VDP_drawText("-C- Activa PRIORIDAD en la Muerte      ", 1, 26);
    VDP_drawText("                                       ", 1, 27);
    }

    if(estado == 3)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS:  ON, MUERTE:  ON", 1, 2);
    VDP_drawText("-A- Pone LUNA recortada en la posicion ", 1, 26);
    VDP_drawText("adecuada y le da prioridad a esos tiles", 1, 27);
    }

    if(estado == 4)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA:  ON, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS:  ON, MUERTE:  ON", 1, 2);
    VDP_drawText("La parte del planoA (ruinas) q coincide", 1, 25);
    VDP_drawText("con la luna, planoB, 'coge' prioridad  ", 1, 26);
    VDP_drawText("Pulsa -B- para continuar               ", 1, 27);
    }

    if(estado == 5)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA:  ON, RUINAS:  ON", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS:  ON, MUERTE:  ON", 1, 2);
    VDP_drawText("Todo con prioridad y por tanto todo con", 1, 25);
    VDP_drawText("con color + orden normal.              ", 1, 26);
    VDP_drawText("Pulsa -C- para continuar               ", 1, 27);
    }

    if(estado == 6)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE: OFF", 1, 2);
    VDP_drawText("Otro ejemplo. Muerte entera pintada con", 1, 25);
    VDP_drawText("con color13 de paleta3, sale oscuro sin", 1, 26);
    VDP_drawText("prioridad (ahora)           Pulsa -A-  ", 1, 27);
    }

   if(estado == 7)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA: OFF, RUINAS: OFF", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE:  ON", 1, 2);
    VDP_drawText("Al poner prioridad, tal y color normal.", 1, 25);
    VDP_drawText("Ahora bien, si pintamos con color14 de ", 1, 26);
    VDP_drawText("la paleta3 (4a paleta) ...  Pulsa -B-  ", 1, 27);
    }

   if(estado == 8)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA:  ON, RUINAS:  ON", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE:  ON", 1, 2);
    VDP_drawText("Muerte entera con color14, con S/H     ", 1, 25);
    VDP_drawText("activo sale brillante y transparente   ", 1, 26);
    VDP_drawText("Pulsa -C- para continuar               ", 1, 27);
    }

   if(estado == 9)
    {
    VDP_drawText("S&H: ON                                ", 1, 0);
    VDP_drawText("PRIORIDAD FONDOS LUNA:  ON, RUINAS:  ON", 1, 1);
    VDP_drawText("PRIORI SPRITES MORRIS: OFF, MUERTE:  ON", 1, 2);
    VDP_drawText("Muerte entera con color15, con S/H     ", 1, 25);
    VDP_drawText("activo sale oscura y transparente      ", 1, 26);
    VDP_drawText("Pulsa -A- para continuar               ", 1, 27);
    }

   if(estado == 10)
    {
    VDP_drawText("    EJEMPLO DE MAPA DE PRIORIDAD       ", 1, 0);
    VDP_drawText("      Pulsa -B- para continuar         ", 1, 1);
    }

   if(estado == 11)
    {
    VDP_drawText("    EJEMPLO DE MAPA DE PRIORIDAD (2)   ", 1, 0);
    VDP_drawText("    Redibuja el fondo linea a linea    ", 1, 1);
    VDP_drawText("      Pulsa -C- para continuar         ", 1, 2);
    }

   if(estado == 12)
    {
    VDP_drawText("    EJEMPLO DE MAPA DE PRIORIDAD (3)   ", 1, 0);
    VDP_drawText("    Redibuja el fondo linea a linea    ", 1, 1);
    VDP_drawText("         de forma dinamica             ", 1, 2);
    VDP_drawText("      UP/DOWN: Cambia la diagonal      ", 1, 3);
    }

}

//simplemente al llegar a un extremo el enemigo se da la vuelta
static void mueveEnemigos()
{
    //X: si se sale de la pantalla, invierte el sentido del movimiento
    muerte_x -= velocidadmovx;
        // Invertir el mov en los extremos de la pantalla (OPCIONAL)
        //muerte_x -= velocidadmovx* enemigoSentidoMovimiento;
        //if(muerte_x>=320 || muerte_x<=-40) enemigoSentidoMovimiento *= -1;

    //Y: mov senoidal
    muerte_y = nivelMedioMuerteY + sinFix16(muerte_x*sinSpeed)*Amplitude;

        //Para que no se salga de la pantalla
        if(muerte_y<=0)   muerte_y = 0;
        if(muerte_y>=224) muerte_y = 224;

    //actualiza la posicion
    SPR_setPosition(spr_Muerte, muerte_x, muerte_y);
}
