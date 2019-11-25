/**
 *      @Title:  Leccion 10 - "Colisiones (2) Bullet Hell mem dinamica optimiz"
 *      @Author: Daniel Bustos "danibus"
 */

#include <genesis.h>
#include "sprites.h"  //carga sprites
#include "fondos.h"   //carga fondos

//Constantes

    #define NAVE_IDLE                0  //sprites de la nave
    #define NAVE_UP                  1
    #define NAVE_DOWN                2
    #define VELOCIDAD_PLAYER         2  //velocidad desplazamiento nave

    #define BLOQUEO_DISPARO          10 //Regula disparos x seg: num ciclos xa desbloquear sig disparo,
    #define BLOQUEO_DISPARO_BOSS     5  //valor mas bajo = balas salen con mas frecuencia.

    #define MAX_BALAS_PLAYER        15  //max num de balas en pantalla
    #define MAX_BALAS_BOSS          40  //max num de balas en pantalla
    #define MAX_EXPLOSIONES         10  //max num de explosiones en pantalla

    #define VELOCIDAD_BALA_PLAYER    6  //valor mas alto = mas rapido
    #define VELOCIDAD_BALA_BOSS      1

    #define CAJA_COL_PLAYER_X1       8 //son coordenadas relativas a la posicion
    #define CAJA_COL_PLAYER_Y1       8 //del sprite. La caja es de 8x8px
    #define CAJA_COL_PLAYER_X2      16 //es decir, 1 tile, está situada justo en
    #define CAJA_COL_PLAYER_Y2      16 //el cuerpo de la nave

    #define CAJA_COL_BOSS_X1       230 //son coordenadas absolutas de la pantalla
    #define CAJA_COL_BOSS_Y1        35 //ya que el boss es en realidad un fondo
    #define CAJA_COL_BOSS_X2       290
    #define CAJA_COL_BOSS_Y2       145

    #define EXPLOSION_T_VIDA        30 //num de ciclos que se repite una explosion antes de eliminarse


//Declaracion de funciones

    static void inicializa_balas_player();
    static void inicializa_explosiones();
    static void inicializa_lista_balas_boss();

    static void crea_bala_player(int tipo);
    static void mantenimiento_balas_player();

    static void crea_explosion(int x, int y);
    static void mantenimiento_explosiones();

    static void crea_bala_boss();
    static void mantenimiento_balas_boss();

    static void actualiza_caja_colisiones_player();

    static void handleInput();
    static void MIDEBUG();


//Declaracion de variables

    //Nave del Player
    struct{
        Sprite *spr_player; //puntero al sprite
        int x, y;           //posición
        int x1,y1,x2,y2;    //caja de colision
        int tempo_disparo;  //tempo de repetición de disparo
    }NAVE;

    //BOSS
    struct{

        //el boss es un fondo, no necesita Sprite*
        //el boss es un fondo, no necesita x,y
        int x1,y1,x2,y2;        //caja de colision
        int tempo_disparo;

    }BOSS;

    //BALAS NAVE
    struct estructura_bala{
        int a, x, y;        // a=activa, x,y=coordenadas.
        int tipo;           // 0=disparo normal, 1=disparo triple
        Sprite *spr;        // sprite
    };
    //declaracion de arrays de structs
    struct estructura_bala lista_balas_player[MAX_BALAS_PLAYER];
    //numero de balas del player
    int num_balas_player;



        //BALAS BOSS
        //creo un struct diferente para no liar el codigo
        //es exactamente igual al normal
        typedef struct estructura_bala2{
            int a, x, y;        // a=activa, x,y=coordenadas.
            int tipo;           // 0=disparo normal, 1=disparo triple
            Sprite *spr;        // sprite
        }bala_boss;
        //declaracion de puntero a arrays de structs
        struct estructura_bala2 *lista_balas_boss;
        //numero balas del boss (para optimizar el bucle)
        int num_balas_boss;


    //EXPLOSIONES (tanto player como boss)
    struct estructura_explosion{
        int a, x, y;        // a=activo, x,y=coordenadas
        int t_vida;         //lo que dura la explosion (en ciclos)
        Sprite *spr;
    }lista_explosiones[MAX_EXPLOSIONES];
    int num_explosiones;

    //total sprites activos
    int total_sprites;





//INICIALIZACIONES
static void inicializa_balas_player(){
    for(int cont = 0; cont <MAX_BALAS_PLAYER; cont++){
        lista_balas_player[cont].a    = 0;
        lista_balas_player[cont].x    = -50; // se crean fuera de pantalla visible de forma que
        lista_balas_player[cont].y    = -50; // al asignar el sprite (abajo), éste no se vea
        lista_balas_player[cont].tipo = 0;
        lista_balas_player[cont].spr  = SPR_addSprite(&bala_sprite, lista_balas_player[cont].x,
                                            lista_balas_player[cont].y, TILE_ATTR(PAL0,TRUE,FALSE, FALSE));
    }
}
static void inicializa_explosiones(){
    for(int cont = 0; cont <MAX_EXPLOSIONES; cont++){
        lista_explosiones[cont].x      = -50;
        lista_explosiones[cont].y      = -50;
        lista_explosiones[cont].spr    = SPR_addSprite(&explosion_sprite, lista_explosiones[cont].x,
                                                     lista_explosiones[cont].y, TILE_ATTR(PAL0,TRUE,FALSE, FALSE));
    }
}
static void inicializa_lista_balas_boss(){

lista_balas_boss = MEM_alloc(MAX_BALAS_BOSS * sizeof(bala_boss));

    for(int cont = 0; cont <MAX_BALAS_BOSS; cont++){
        lista_balas_boss[cont].a    = 0;
        lista_balas_boss[cont].x    = -50;
        lista_balas_boss[cont].y    = -50;
        lista_balas_boss[cont].tipo = 0;
        lista_balas_boss[cont].spr  = SPR_addSprite(&bala_sprite, lista_balas_boss[cont].x,
                                                    lista_balas_boss[cont].y, TILE_ATTR(PAL0,TRUE,FALSE, FALSE));
    }

num_balas_player = 0;
num_balas_boss   = 0;
num_explosiones  = 0;
total_sprites    = 0;
}



//CREA BALA PLAYER
//crea una bala en la primera pos libre del array
//tipo: 0=disparo normal, 1=disparo triple
static void crea_bala_player(int tipo)
{
    for(int cont = 0; cont <MAX_BALAS_PLAYER; cont++)
    {
        if(lista_balas_player[cont].a == 0)
        {
                lista_balas_player[cont].a = 1;
                lista_balas_player[cont].x = NAVE.x+30;
                lista_balas_player[cont].y = NAVE.y+15;
                lista_balas_player[cont].tipo = tipo;
                num_balas_player++;
                total_sprites++;
                break;
        }
    }
}


//VIDA DE UNA BALA DEL PLAYER
//- Mueve bala según su tipo (recto o en diagonal)
//- Elimina la bala si se sale de la pantalla
//- Elimina la bala si toca la caja de colision del enemigo
static void mantenimiento_balas_player()
{
    for(int cont = 0; cont <MAX_BALAS_PLAYER; cont++)
    {
        if(lista_balas_player[cont].a != 0) //solo las activas
        {
            if(lista_balas_player[cont].tipo==0)       lista_balas_player[cont].x+=VELOCIDAD_BALA_PLAYER;
            else if(lista_balas_player[cont].tipo==1){ lista_balas_player[cont].x+=VELOCIDAD_BALA_PLAYER; lista_balas_player[cont].y--; }
            else if(lista_balas_player[cont].tipo==2){ lista_balas_player[cont].x+=VELOCIDAD_BALA_PLAYER; lista_balas_player[cont].y++; }

            //sale de la pantalla
            if(lista_balas_player[cont].x>=330 || lista_balas_player[cont].y<=-20 || lista_balas_player[cont].y>=240)
            {
                lista_balas_player[cont].a = 0 ;  //marca como inactivo en el vector
                num_balas_player--;               //una bala menos
                total_sprites--;
                continue;                         //esta ya fuera de la zona visible, sigue el FOR en la sig iteracion
            }
            //choca con la caja de colision del boss
            if(lista_balas_player[cont].x>BOSS.x1 && lista_balas_player[cont].x<BOSS.x2 &&
               lista_balas_player[cont].y>BOSS.y1 && lista_balas_player[cont].y<BOSS.y2)
            {
                crea_explosion(lista_balas_player[cont].x,lista_balas_player[cont].y);

                lista_balas_player[cont].a = 0 ;
                lista_balas_player[cont].x = -50;
                lista_balas_player[cont].y = -50;
                num_balas_player--;
                total_sprites--;
            }
            //situa la bala donde toque
            SPR_setPosition(lista_balas_player[cont].spr,lista_balas_player[cont].x,lista_balas_player[cont].y );
        }
    }
}

//CREA EXPLOSION en la pos x,y
static void crea_explosion(int x, int y)
{
    for(int cont = 0; cont <MAX_EXPLOSIONES; cont++)
    {
        if(lista_explosiones[cont].a == 0)
        {
                lista_explosiones[cont].a = 1;
                lista_explosiones[cont].x = x;
                lista_explosiones[cont].y = y;
                lista_explosiones[cont].t_vida = EXPLOSION_T_VIDA;
                SPR_setPosition(lista_explosiones[cont].spr,lista_explosiones[cont].x,lista_explosiones[cont].y );
                num_explosiones++;
                total_sprites++;
                break;
        }
    }
}


//VIDA DE UNA EXPLOSION
// Cada ciclo disminuye su t_vida, al llegar a cero se elimina
static void mantenimiento_explosiones()
{
    for(int cont = 0; cont <MAX_EXPLOSIONES; cont++)
    {
        if(lista_explosiones[cont].a != 0) //solo las activas
        {
            lista_explosiones[cont].t_vida--;
            if(lista_explosiones[cont].t_vida<=0)
            {
                lista_explosiones[cont].a = 0 ;                   //marca como inactivo en el vector
                lista_explosiones[cont].x = -100;                 //posición (fuera de la pantalla), falta decirle
                lista_explosiones[cont].y = -100;                 //al VDP que actualice la posicion en la tabla de sprites
                SPR_setPosition(lista_explosiones[cont].spr,lista_explosiones[cont].x,lista_explosiones[cont].y );
                num_explosiones--;
                total_sprites--;
            }
        }
    }
}




//CREA BALA DEL BOSS
//NO CREA, activa (a=1) la bala de la primera pos libre del array
//La bala se situa en una posición vertical aleatoria y con x=220
//El sprite ya lo tenía asignado previamente
static void crea_bala_boss()
{
    for(int cont = 0; cont <MAX_BALAS_BOSS; cont++)
    {
        if(lista_balas_boss[cont].a == 0)
        {
            lista_balas_boss[cont].a=1;
            lista_balas_boss[cont].tipo=(((random()%3)-1)+1); // da 0,1 y 2
            lista_balas_boss[cont].x=220;
            lista_balas_boss[cont].y=(((random()%200)-1)+1);  //200 es el max de la coordenada y
            num_balas_boss++;
            total_sprites++;
            break;
        }
    }
}


//VIDA DE UNA BALA DEL BOSS
//- Mueve bala según su tipo: 0,1,2: recto, diagonal-izq-arriba, diagonal-izq-abajo
//- Elimina la bala si se sale de la pantalla o si toca la caja de colision del player
//- Cuando dentro del bucle tratamos tantas balas como balas activas, salimos del bucle
static void mantenimiento_balas_boss()
{
    for(int cont = 0, i=0; cont < MAX_BALAS_BOSS; cont++)
    {
        if(lista_balas_boss[cont].a == 1)    //solo las activas
        {
            i++; if(i>num_balas_boss) break; //no vamos a hacer más ciclos del bucle que los necesarios

            if(lista_balas_boss[cont].tipo==0)  lista_balas_boss[cont].x-=VELOCIDAD_BALA_BOSS;
            if(lista_balas_boss[cont].tipo==1){ lista_balas_boss[cont].x-=VELOCIDAD_BALA_BOSS; lista_balas_boss[cont].y--; }
            if(lista_balas_boss[cont].tipo==2){ lista_balas_boss[cont].x-=VELOCIDAD_BALA_BOSS; lista_balas_boss[cont].y++; }

            //sale de la pantalla
            if(lista_balas_boss[cont].x<=0 || lista_balas_boss[cont].y<=0 || lista_balas_boss[cont].y>=220)
            {
                lista_balas_boss[cont].a = 0;       //marca como inactivo en el vector
                lista_balas_boss[cont].x = -50;     //fuera de la pantalla
                lista_balas_boss[cont].y = -50;
                num_balas_boss--;
                total_sprites--;

            }
            //choca con la caja de colision del player
            else if( lista_balas_boss[cont].x>NAVE.x1 && lista_balas_boss[cont].x<NAVE.x2 &&
                     lista_balas_boss[cont].y>NAVE.y1 && lista_balas_boss[cont].y<NAVE.y2    )
                {
                    crea_explosion(lista_balas_boss[cont].x,lista_balas_boss[cont].y);
                    lista_balas_boss[cont].a = 0;
                    lista_balas_boss[cont].x = 0;
                    lista_balas_boss[cont].y = -50;
                    num_balas_boss--;
                    total_sprites--;
                }

             SPR_setPosition(lista_balas_boss[cont].spr,lista_balas_boss[cont].x,lista_balas_boss[cont].y );
        }
    }
}


//CAJA COLISION PLAYER
static void actualiza_caja_colisiones_player()
 {
    NAVE.x1 = NAVE.x + CAJA_COL_PLAYER_X1;
    NAVE.y1 = NAVE.y + CAJA_COL_PLAYER_Y1;
    NAVE.x2 = NAVE.x + CAJA_COL_PLAYER_X2;
    NAVE.y2 = NAVE.y + CAJA_COL_PLAYER_Y2;
 }



//Recoge la entrada del mando y actualiza la posicion de la nave
static void handleInput()
{
    //variable donde se guarda la entrada del mando
    u16 value = JOY_readJoypad(JOY_1);
    //si pulsamos izquierda...
    if (value & BUTTON_LEFT)
        SPR_setPosition(NAVE.spr_player, NAVE.x-=VELOCIDAD_PLAYER, NAVE.y);
    //si pulsamos derecha...
    if (value & BUTTON_RIGHT)
        SPR_setPosition(NAVE.spr_player, NAVE.x+=VELOCIDAD_PLAYER, NAVE.y);
    //si pulsamos arriba...
    if (value & BUTTON_UP)
    {   SPR_setPosition(NAVE.spr_player, NAVE.x, NAVE.y-=VELOCIDAD_PLAYER); SPR_setAnim(NAVE.spr_player, NAVE_UP);   }
    //si pulsamos abajo...
    if (value & BUTTON_DOWN)
    {   SPR_setPosition(NAVE.spr_player, NAVE.x, NAVE.y+=VELOCIDAD_PLAYER); SPR_setAnim(NAVE.spr_player, NAVE_DOWN); }
        //si no pulsamos
        if ((!(value & BUTTON_UP)) && (!(value & BUTTON_DOWN)))             SPR_setAnim(NAVE.spr_player, NAVE_IDLE);

    //limites
    if(NAVE.x <= 0)      NAVE.x =   2;
    if(NAVE.x >= 320-32) NAVE.x = 320-32-2;
    if(NAVE.y <= 0)      NAVE.y =   2;
    if(NAVE.y >= 224-24) NAVE.y = 224-24-2;

    //si pulsamos A: PLAYER disparo normal
    if (value & BUTTON_A && NAVE.tempo_disparo == 0){
        crea_bala_player(0);
        NAVE.tempo_disparo = 1; //para bloquear el disparo durante un tiempo limitado (y que no salgan 60disparos/seg)
    }
        //para liberar el disparo despues de pulsar A
        if(NAVE.tempo_disparo>0) NAVE.tempo_disparo++;
        if(NAVE.tempo_disparo>BLOQUEO_DISPARO) NAVE.tempo_disparo = 0;


    //si pulsamos B: PLAYER disparo triple
    if (value & BUTTON_B && NAVE.tempo_disparo == 0){
        crea_bala_player(0);
        crea_bala_player(1);
        crea_bala_player(2);
        NAVE.tempo_disparo = 1; //para bloquear el disparo durante un tiempo limitado (y que no salgan 60disparos/seg)
    }

    //si pulsamos C
    if (value & BUTTON_C && (BOSS.tempo_disparo == 0 || BOSS.tempo_disparo == 1)){
        crea_bala_boss();

        BOSS.tempo_disparo++;
    }
        //para liberar el disparo despues de pulsar C
        if(BOSS.tempo_disparo>0) BOSS.tempo_disparo++;
        if(BOSS.tempo_disparo>BLOQUEO_DISPARO_BOSS) BOSS.tempo_disparo = 0;


            //DEBUG
            if (value & BUTTON_START)
            MEM_free(lista_balas_boss);


}



//MIDEBUG: Muestra num de sprites en pantalla
static void MIDEBUG()
{
  //show total sprite number
  char mi_string[32];
  sprintf(mi_string, "Total sprites: %4d", total_sprites);
  VDP_drawText(mi_string, 1, 24);
  //fps
  VDP_showFPS(FALSE);
}





int main()
{
    //pone la pantalla a 320x224
    VDP_setScreenWidth320();

    //inicializa motor de sprites
    SPR_init(0, 0, 0);

    //Inicializa estructuras a CERO
    inicializa_balas_player();
    inicializa_explosiones();
    inicializa_lista_balas_boss();

    //Inicializa variables
    NAVE.x = 64;
    NAVE.y = 145;
    NAVE.tempo_disparo = 0;

    BOSS.tempo_disparo  = 0;
    BOSS.x1 = CAJA_COL_BOSS_X1; //al ser fijas, no es necesario
    BOSS.y1 = CAJA_COL_BOSS_Y1; //recalcular cada frame como con
    BOSS.x2 = CAJA_COL_BOSS_X2; //la nave
    BOSS.y2 = CAJA_COL_BOSS_Y2;

    //variable para llevar el control de tiles
    u16 ind;

    //recoge la paleta de bala, player y fondo (compartida)
    VDP_setPalette(PAL0,nave_sprite.palette->data);

    //fondos
    ind = TILE_USERINDEX;
    VDP_drawImageEx(PLAN_B, &fondo2, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo2.tileset->numTile;
    VDP_drawImageEx(PLAN_A, &fondo1, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += fondo1.tileset->numTile;

    //configura el scroll
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    int scrollValueVariable = 0;

    //crea la nave del player
    NAVE.spr_player = SPR_addSprite(&nave_sprite, NAVE.x,  NAVE.y,  TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    //AYUDA en pantalla
    VDP_drawText("  fps  BULLET HELL HELP  ",  2, 1);
    VDP_drawText("PAD - Controles, A/B: Fire",  2, 2);
    VDP_drawText("C: Boss Bullet Hell       ",  2, 3);


    //Bucle principal
    while(TRUE)
    {
        //muestra cuantos sprites hay en pantalla
        MIDEBUG();

        //recoge la entrada de los mandos
        handleInput();

        //para las colisiones
        actualiza_caja_colisiones_player();

        //turno balas player: mueve, colisiona...
        mantenimiento_balas_player();

        //turno balas boss
        mantenimiento_balas_boss();

        //turno explosiones
        mantenimiento_explosiones();

        //mueve el plano B (fondo de estrellas)
        scrollValueVariable-=3;
        VDP_setHorizontalScroll(PLAN_B, scrollValueVariable);

        //actualiza el VDP
        SPR_update();

        //sincroniza la Megadrive con la TV
        VDP_waitVSync();
    }

    return 0;
}
