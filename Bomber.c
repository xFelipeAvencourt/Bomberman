#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// CONSTANTES JANELA DO JOGO
#define SCREENWIDTH 1020 // LARGURA DA TELA
#define SCREENHEIGHT 660 // ALTURA DA TELA

// CONSTANTES PERSONAGEM/INIMGOS
#define VEL_PERSONAGEM 5
#define GIF 4
#define LADO 60
#define TAM_CEL 20
#define N_BOMBAS 3
#define N_BOMBAS_MAX 5
#define N_INIMIGOS 5
#define RAIO 2

// CONSTANTES PAREDES
#define N_LINHAS  ((SCREENHEIGHT-LADO)/(2*LADO))
#define N_COLUNAS ((SCREENWIDTH-LADO)/(2*LADO))
#define N_PAREDES N_LINHAS * N_COLUNAS
#define N_MUROS N_PAREDES
#define CHANCE_SPAWN 5
#define BARREIRA 4
#define TAM_PORTA LADO/2

//CONSTANTES DE TEXTO/IMAGEM
#define TAM_GAME_OVER 80
#define TAM_MSG_CONTINUAR 20
#define HUD 40
#define TAM 30
#define MULTIPLICADOR 1000
#define N_CORES 10

// CONSTANTES DE RANKING
#define MULTIPLICADOR 1000
#define N_RANKING 5 // ARQUIVAR ATE A POSIÇÃO:


typedef enum{
    guardada,
    posicionada,
    ativa
}STATUSBOMBAS;

typedef struct{
    STATUSBOMBAS stats;
    Rectangle posicao;
}BOMBAS;

typedef struct{
    char nome[TAM];
    float pontuacao;
    double tempo;
}DADOS;

typedef struct{
    Rectangle personagem;
    short vidas;
    BOMBAS bombs[N_BOMBAS_MAX];
}JOGADOR;

typedef struct{
    Rectangle box;
    short status;
}ENEMY;

typedef enum{
destruido,
intacto
}STATUSMURO;

typedef struct{
    Rectangle lugar;
    short indice;
}PORTA;

typedef struct{
Rectangle formato;
STATUSMURO status;
PORTA portal;
}MURO;

typedef struct{
    MURO tijolos[N_MUROS];
    Rectangle paredesFixas[N_PAREDES];
    Rectangle limites[BARREIRA];
    Color CorFase[N_CORES];
}MAPA;


// INICIALIZACAO DO JOGO:
void CarregarTexturas(MAPA *map, JOGADOR *player, ENEMY inimigos[],int *num_inimigo); // CARREGAR AS TEXTURAS
void HUD_game(short *pause, double *tempo, JOGADOR *personagem, short *fase); // INTERFACE DO JOGO
void ReiniciarFase(JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause, int *sorteado, int *num_inimigo); // REINICIAR INIMIGOS, MUROS DA FASE

// INICIALIZACAO DA FASE:
void CriarLimites(MAPA *map); // CRIAR PARADES LIMITES DO MAPA
void CriarMuros (MAPA *map, JOGADOR *player, ENEMY inimigos[],Rectangle *porta, int *num_inimigos,int *num_muros); // CRIA AS POSICOES DOS MUROS E GERA A PORTA PARA CADA FASE
short TemMuro(MURO muros[], int *tam, Rectangle *quadrado);
short TemParede(Rectangle paredesFixas[], Rectangle *quadrado);
short Enemy(ENEMY inimigos[], Rectangle *quadrado);
void CriarParedes(MAPA *map); // CRIAR PAREDES FIXAS DO MAPA
void SpawnInimigo(ENEMY inimigo[], MAPA *map, int *n_inimigos); // SPAWNAR INIMIGOS

// DESENHAR NO LOOP:
void GerarLimites(MAPA *map); // DESENHA OS LIMITES NO MAPA
void GerarParedesFixas(MAPA *map); // DESENHAR PAREDES FIXAS NO MAPA
void GerarMuros(MAPA *map, Rectangle *porta,int *num_muros); // DESENHAR MUROS NO MAPA
void DesenharInimigos(ENEMY inimigos[],int *num_inimigos); // DESENHAR INIMIGO NO MAPA
void DesenharBomba(JOGADOR *player, short *acm_bomba); // DESENHA AS BOMBAS POSICIONADAS NO MAPA

// DESENHAR NO LOOP (MOVIMENTO):
void Movimento(JOGADOR *player, MAPA *map, short *achou_porta, Rectangle *porta, ENEMY inimigos[], int *num_inimigos,int *num_muros); // MOVIMENTO DO PERSONAGEM
void EncontraPorta(JOGADOR *player, short *achou_porta, Rectangle *porta, ENEMY inimigos[], int *num_inimigos);
short Colisao(Rectangle *ghost, MAPA *map,int *num_muros); // TESTAR COLISAO DE CELULA COM PAREDES E MUROS
void Perseguir(ENEMY inimigos[], JOGADOR *player, MAPA *map, int *num_inimigos,int* n_muros); // CHAMAR PERSEGUE COM CADA INIMIGO
void Persegue(ENEMY *enemy, Rectangle *personagem, MAPA *map,int *n_muros); // PERSEGUE UM INIMIGO AO PERSONAGEM
void Morreu (JOGADOR *player, ENEMY inimigos[], short *pause); // TESTA SE ALGUM INIMIGO COLIDIU COM O PLAYER


// BOMBA E INTERAÇÃO COM MAPA
void BotarBomba (JOGADOR *player, short *acm_bomba,short *bombas_mais); // BOTAR BOMBA NO LOCAL DO PERSONAGEM
void Detonar (JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause,int *n_muros); // DETONAR TODAS AS BOMBAS POSICIONADAS, CHAMANDO EXPLOSAO
void Explosao (BOMBAS *bomba, JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause,int *n_muros); // RECEBE UMA BOMBA DE DETONADA POR VEZ, CHAMANDO FIRE
short Fire(BOMBAS *bomba, JOGADOR *player, MAPA *map, ENEMY inimigos[], short *pause,int *n_muros); //FAZ O RAIO DE EXPLOSAO DA BOMBA, CHECANDO COM OBJETOS DO MAPA
void MatarEnemy(BOMBAS *bomba, ENEMY inimigos[]); // MATAR INIMIGOS CASO ESTEJA NO RAIO DA BOMBA
short PercorrerLimite(BOMBAS *bomba, Rectangle limites[]); // VERIFICAR SE TEM LIMITE NO RAIO DA BOMBA
short PercorrerFixa(BOMBAS *bomba, Rectangle paredesFixas[]); // VERIFICAR SE TEM PAREDE FIXA NO RAIO DA BOMBA
short PercorrerMuro(BOMBAS *bomba, MURO tijolos[],int *n_muros); // VERIFICAR SE TEM MUROS DESTRUIVEIS NO RAIO DA BOMBA


////////////////////////////

short Jogo(DADOS *pessoa);
short Backup(DADOS players[]);
void Classificar(DADOS *pessoa, short fase);
void Ordenar(DADOS players[], short *jogadas);
void Ranking(DADOS players[], short *jogadas);
void Copiar(char string1[], char string2[]);
void GerarRanking(DADOS players[], short *jogadas);
short Jogo(DADOS *pessoa)


{
    int num_muros = N_MUROS/2;
    JOGADOR player;
    MAPA map;
    ENEMY inimigos[N_INIMIGOS];
    short pause=0, acm_bomba=0, fase=0;
    int sorteado;
    Rectangle porta = {0,0,0,0};
    short bombas_mais = 0;
    int num_inimigo = N_INIMIGOS - 3;
    short achou_porta = 0;
    player.vidas = 3;

    CarregarTexturas(&map,&player,inimigos,&num_inimigo);
    CriarMuros(&map,&player,inimigos,&porta,&num_inimigo,&num_muros);

    while (!WindowShouldClose() && player.vidas > 0) // FECHAR JANELA CASO APERTE ESQ OU ACABAR AS VIDAS
    {
        BeginDrawing();
        ClearBackground(map.CorFase[fase%7]);
        GerarLimites(&map);
        GerarParedesFixas(&map);
        GerarMuros(&map,&porta,&num_muros);
        HUD_game(&pause,&(*pessoa).tempo,&player,&fase);
        if(pause==0 && achou_porta ==0)
        {
            Movimento(&player,&map,&achou_porta,&porta,inimigos,&num_inimigo,&num_muros);
            DesenharInimigos(inimigos, &num_inimigo);
            Perseguir(inimigos,&player,&map,&num_inimigo,&num_muros);
            Morreu(&player,inimigos,&pause);
            BotarBomba(&player,&acm_bomba,&bombas_mais);
            DesenharBomba(&player,&acm_bomba);
            if(IsKeyPressed(KEY_ENTER))
                Detonar(&player,&map,inimigos,&acm_bomba,&pause,&num_muros);
            if(IsKeyPressed(KEY_P))
                pause=2;
        }
         else
        {
           if(IsKeyDown(KEY_BACKSPACE)&& pause==1)
            {
                player.vidas--;
                ReiniciarFase(&player,&map,inimigos,&acm_bomba,&pause,&sorteado,&num_inimigo);
                CriarMuros(&map,&player,inimigos,&porta,&num_inimigo,&num_muros);
            }
            if(achou_porta==1)
            {
                fase++;
                if(fase < 4)
                {
                    num_inimigo++;
                    num_muros+=5;
                    if(fase < 3)
                        bombas_mais++;

                }

                achou_porta=0;
                ReiniciarFase(&player,&map,inimigos,&acm_bomba,&pause,&sorteado,&num_inimigo);
                CriarMuros(&map,&player,inimigos,&porta,&num_inimigo,&num_muros);
            }


        }
        EndDrawing();
    }
    CloseWindow();

    if(fase)
        (*pessoa).pontuacao = MULTIPLICADOR * ( fase / (*pessoa).tempo);
    return fase;
}



// CARREGAR TEXTURAS DO JOGO: CARREGA AS DIMENCOES & TEXTURAS DO JOGO

void CarregarTexturas(MAPA *map, JOGADOR *player, ENEMY inimigos[],int *num_inimigo)
{

    CriarLimites(map);
    CriarParedes(map);

    (*map).CorFase[0]=WHITE;
    (*map).CorFase[1]=LIME;
    (*map).CorFase[2]=DARKBROWN;
    (*map).CorFase[3]=DARKGRAY;
    (*map).CorFase[4]=DARKGREEN;
    (*map).CorFase[5]=DARKPURPLE;
    (*map).CorFase[6]=BEIGE;
    (*map).CorFase[7]=BROWN;

    SpawnInimigo(inimigos,map,num_inimigo);

    (*player).personagem.width=LADO -(LADO/5);
    (*player).personagem.height=LADO -(LADO/5);
    (*player).personagem.x=(SCREENWIDTH-LADO)/2 + 1; // + 1 para nao nascer grudado na parede central
    (*player).personagem.y=(SCREENHEIGHT-LADO)/2 + 1;// + 1 para nao nascer grudado na parede central
}

// INTERFACE DO JOGO: ESCRITAS PARA A INTERFACE DO JOGO
void HUD_game(short *pause, double *tempo, JOGADOR *personagem, short *fase)
{
    *tempo=GetTime();

    DrawText(TextFormat("TEMPO: %.1f ",*tempo ),LADO/2, (LADO-HUD)/2, HUD, DARKBLUE);
    DrawText(TextFormat("FASE: %d ", *fase + 1),LADO/2+(MeasureText("TEMPO:",2*HUD)), (LADO-HUD)/2, HUD, DARKBLUE);
    DrawText(TextFormat("VIDAS: %d", (*personagem).vidas), SCREENWIDTH-MeasureText("VIDAS:",HUD)-2*LADO,SCREENHEIGHT-LADO/2 - 20,HUD,RED);
    if(*pause==1)
    {
        DrawText("GAME OVER", (SCREENWIDTH-MeasureText("GAME OVER",TAM_GAME_OVER))/2, (SCREENHEIGHT/2)-TAM_GAME_OVER, TAM_GAME_OVER, RED);
        DrawText("Pressione APAGAR para continuar ou ESC para sair",
                (SCREENWIDTH-MeasureText("Pressione APAGAR para continuar ou ESC para sair",TAM_MSG_CONTINUAR))/2,
                 SCREENHEIGHT/2, TAM_MSG_CONTINUAR, BLACK);
    }
    if(*pause==2)
    {
        DrawText("PAUSE", (SCREENWIDTH-MeasureText("PAUSE",TAM_GAME_OVER))/2, (SCREENHEIGHT/2)-TAM_GAME_OVER, TAM_GAME_OVER+6, BLACK);
        DrawText("PAUSE", (SCREENWIDTH-MeasureText("PAUSE",TAM_GAME_OVER))/2+4, (SCREENHEIGHT/2)-TAM_GAME_OVER+4, TAM_GAME_OVER, RED);
        DrawText("Pressione MOVE para continuar ou ESC para sair",
                (SCREENWIDTH-MeasureText("Pressione MOVE para continuar ou ESC para sair",TAM_MSG_CONTINUAR))/2,
                 SCREENHEIGHT/2, TAM_MSG_CONTINUAR, BLACK);
        if(IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W) ||
           IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S) ||
           IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A) ||
           IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        {
            *pause=0;
        }
    }
}

// CRIAR LIMITES DO MAPA: TAMANHO DOS LIMTES DO MAPA

void CriarLimites (MAPA *map)
{
    // TOPO
    (*map).limites[0].x = 0;
    (*map).limites[0].y = 0;
    (*map).limites[0].height = LADO;
    (*map).limites[0].width = SCREENWIDTH;


    // DIREITA
    (*map).limites[1].x = SCREENWIDTH-LADO;
    (*map).limites[1].y = LADO;
    (*map).limites[1].height = SCREENHEIGHT-(2*LADO);
    (*map).limites[1].width = LADO;

    // BAIXO
    (*map).limites[BARREIRA-2].x = 0;
    (*map).limites[BARREIRA-2].y = SCREENHEIGHT-LADO;
    (*map).limites[BARREIRA-2].height = LADO;
    (*map).limites[BARREIRA-2].width = SCREENWIDTH;

    // ESQUERDA
    (*map).limites[BARREIRA-1].x = 0;
    (*map).limites[BARREIRA-1].y = LADO;
    (*map).limites[BARREIRA-1].height = SCREENHEIGHT-(2*LADO);
    (*map).limites[BARREIRA-1].width = LADO;
}

// CRIAR MUROS DESTRUIVEIS: ALOCA AS POSICOES DOS MUROS, GERA A PORTA EM UM DOS INDICES

void CriarMuros (MAPA *map, JOGADOR *player, ENEMY inimigos[],Rectangle *porta, int *num_inimigos,int *num_muros)
{
    int random;
    int lin, col, ind=0;
    Rectangle safe = { (SCREENWIDTH/2)-(RAIO*LADO), (SCREENHEIGHT/2), (2*RAIO*LADO), 2}; // ZONA DE SPAWN DO PERSONAGEM

     do
    {
        col = 1 + (int) (rand() / (double) RAND_MAX * ((2*N_COLUNAS-1)-1)); // X
        lin = 1 + (int) (rand() / (double) RAND_MAX * ((2*N_LINHAS-1)-1)); // Y
        if( (lin%2 == 1 || (lin%2 == 0 && col%2 == 1)) ) // LUGARES PARAEDES FIXAS
        {
            if(!CheckCollisionRecs((Rectangle){col*LADO,lin*LADO,LADO,LADO},safe) && !Enemy(inimigos,&(Rectangle){col*LADO,lin*LADO,LADO,LADO})
               && !TemMuro((*map).tijolos,&ind,&(Rectangle){col*LADO,lin*LADO,LADO,LADO}) ) // NÃO ESTA NA SAFE E NÃO TEM INIMIGOS
            {
                random = (int) (rand() / (double) RAND_MAX * (CHANCE_SPAWN + 1));
                if(random == 1)
                {
                    (*map).tijolos[ind].formato.x = col*LADO;
                    (*map).tijolos[ind].formato.y = lin*LADO;
                    (*map).tijolos[ind].formato.width = LADO;
                    (*map).tijolos[ind].formato.height = LADO;
                    (*map).tijolos[ind].status=intacto;
                    (*map).tijolos[ind].portal.indice=0;
                    ind++;
                }
            }
        }

    }while(ind<*num_muros);
    // SORTEIO DA PORTA:
    random = (int) (rand() / (double) RAND_MAX * *num_muros);
    (*map).tijolos[random].portal.indice=1;
    *porta = (Rectangle){(*map).tijolos[random].formato.x,(*map).tijolos[random].formato.y,TAM_PORTA,TAM_PORTA*2};


}

// TESTAR COLISÃO: TESTA COLISAO COM BLOCOS DO MAPA

short Colisao (Rectangle *celula, MAPA *map,int *num_muros)
{

    if(TemMuro((*map).tijolos,num_muros,celula))
        return 1;

    if(TemParede(map->paredesFixas, celula))
        return 1;
    return 0;
}


// CHECAGEM DE COLISÃO COM TODOS MUROS
short TemMuro(MURO muros[], int *tam, Rectangle *quadrado)
{
    int i=0;
    short encontrado = 0;

    while(!encontrado && i<*tam)
    {
        if(CheckCollisionRecs(*quadrado,muros[i].formato))
           return 1;
        i++;
    }
    return 0;
}

short TemParede(Rectangle paredesFixas[], Rectangle *quadrado)
{
    int i=0;
    short encontrado = 0;

    while(!encontrado && i<N_PAREDES)
    {
        if(CheckCollisionRecs(*quadrado,paredesFixas[i]))
           return 1;
        i++;
    }
    return 0;
}
// TEM INIMIGO NO BLOCO

short Enemy(ENEMY inimigos[], Rectangle *quadrado)
{
    int i;

    for(i=0;i<N_INIMIGOS;i++)
        if(CheckCollisionRecs(*quadrado,inimigos[i].box))
            return 1; // ESTA ONDE TEM INIMIGOS

    return 0;
}

// CRIAR PAREDES INTERNAS: ALOCA AS POSICCOES DAS PAREDES FIXAS

void CriarParedes (MAPA *map)
{
    int lin, col, p=0;

    for(lin=1;lin<N_LINHAS;lin++)
        for(col=1;col<N_COLUNAS;col++)
        {
            (*map).paredesFixas[p].x = col*2*LADO;
            (*map).paredesFixas[p].y = lin*2*LADO;
            (*map).paredesFixas[p].width = LADO;
            (*map).paredesFixas[p].height = LADO;
            p++;
        }
}

// SPAWNAR INIMIGO: SPAWNA INIMIGOS EM LUGARES POSSIVEIS

void SpawnInimigo(ENEMY inimigo[], MAPA *map, int *n_inimigos)
{
    int rx = 0,ry = 0;
    int enemy=0;
    short spawn=0;
    Rectangle safe = { (SCREENWIDTH/2)-(RAIO*LADO), (SCREENHEIGHT/2), (2*RAIO*LADO), 2}; // ZONA DE SPAWN DO PERSONAGEM


    srand(time(0));
    while(enemy<*n_inimigos)
    {
        inimigo[enemy].box.height=LADO/1.5;
        inimigo[enemy].box.width=LADO/1.5;
        inimigo[enemy].status = 1;
        do
        {
            rx = 1 + (int) (rand() / (double) RAND_MAX * ((2*N_COLUNAS-1)-1));
            ry = 1 + (int) (rand() / (double) RAND_MAX * ((2*N_LINHAS-1)-1));
            if( ((ry %2 == 1) || (ry %2 == 0 && rx %2 == 1)) && !CheckCollisionRecs((Rectangle){rx*LADO,ry*LADO,LADO,LADO},safe))
            {
               inimigo[enemy].box.x = rx*LADO;
               inimigo[enemy].box.y =  ry*LADO;
               enemy++;
               spawn=1;
            }
        }while(spawn==0);
    }
}

// GERAR LIMITES: DESENHA LIMITES DO MAPA

void GerarLimites (MAPA *map)
{
    short i;

    for(i=0;i<BARREIRA;i++)
        DrawRectangleRec((*map).limites[i],LIGHTGRAY);
}

// GERAR PAREDES FIXAS: DESENHA PAREDES FIXAS

void GerarParedesFixas(MAPA *map)
{
    int p;
    for(p=0;p<N_PAREDES;p++)
        DrawRectangleRec((*map).paredesFixas[p],GREEN);
}

// GERAR MUROS: DESENHA MUROS DESTRUIVEIS DO MAPA

void GerarMuros (MAPA *map, Rectangle *porta,int *num_muros)
{
    int j;

    for(j=0;j<*num_muros;j++)
    {
        if((*map).tijolos[j].status != destruido)
        {
            if((*map).tijolos[j].portal.indice == 0)
                DrawRectangleRec((*map).tijolos[j].formato,BLUE);
            if((*map).tijolos[j].portal.indice == 1)
                DrawRectangleRec((*map).tijolos[j].formato,YELLOW);
        }
        else
            if((*map).tijolos[j].portal.indice==1)
                DrawRectangleRec(*porta,ORANGE);

    }
}

// DESENHAR INIMIGOS:

void DesenharInimigos(ENEMY inimigos[],int *num_inimigos)
{
    int i;

    for(i=0;i<*num_inimigos;i++)
        DrawRectangleRec(inimigos[i].box,BLACK);
}

// DESENHAR BOMBA: DESENHA BOMBAS ATIVAS

void DesenharBomba(JOGADOR *player, short *acm_bomba)
{
    short i;

    for(i=0;i<*acm_bomba;i++)
    {
        DrawRectangleRec((*player).bombs[i].posicao,RED);
    }
}

// MOVIMENTO DE PERSONAGEM:

void Movimento(JOGADOR *player, MAPA *map, short *achou_porta, Rectangle *porta, ENEMY inimigos[], int *num_inimigos, int*num_muros)
{
    Rectangle ghost = {(*player).personagem.x, (*player).personagem.y, (*player).personagem.width, (*player).personagem.height};

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        ghost.y-=VEL_PERSONAGEM;
        if(!CheckCollisionRecs(ghost,(*map).limites[0]))
         if(!Colisao(&ghost,map,num_muros))
            (*player).personagem.y -= VEL_PERSONAGEM;
    }
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        ghost.y+=VEL_PERSONAGEM;
        if(!CheckCollisionRecs(ghost,(*map).limites[BARREIRA-2]))
         if(!Colisao(&ghost,map,num_muros))
           (*player).personagem.y += VEL_PERSONAGEM;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        ghost.x+=VEL_PERSONAGEM;
         if(!CheckCollisionRecs(ghost,(*map).limites[1]))
          if(!Colisao(&ghost,map,num_muros))
            (*player).personagem.x += VEL_PERSONAGEM;
    }
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        ghost.x-=VEL_PERSONAGEM;
        if(!CheckCollisionRecs(ghost,(*map).limites[BARREIRA-1]))
         if(!Colisao(&ghost,map,num_muros))
           (*player).personagem.x -= VEL_PERSONAGEM;
    }

    DrawRectangleRec((*player).personagem,MAROON);
    EncontraPorta(player, achou_porta, porta, inimigos, num_inimigos);



}
// INTERAÇÃO DO JOGADOR COM A PORTA: PASSAGEM DE FASE
void EncontraPorta(JOGADOR *player, short *achou_porta, Rectangle *porta, ENEMY inimigos[], int *num_inimigos)
{
    int i;
    short t_mortos = 1;
    for(i=0;i<*num_inimigos;i++)
        if(inimigos[i].status == 1)
            t_mortos = 0;
    if(t_mortos == 1)
        if(CheckCollisionRecs((*player).personagem,*porta))
            *achou_porta = 1;
}

// PERSEGUIR PERSONAGEM

void Perseguir(ENEMY inimigos[], JOGADOR *player, MAPA *map, int *num_inimigos,int*n_muros)
{
    int i;

    ENEMY *enemy_1;

    for(i=0;i<*num_inimigos;i++)
    {
        enemy_1 = &inimigos[i];
        //if(enemy_1.sta)
        Persegue(enemy_1,&(*player).personagem,map,n_muros);
    }
}

// PERSEGUE PERSONAGEM

void Persegue(ENEMY *enemy, Rectangle *personagem, MAPA *map,int *n_muros)
{
    if((*enemy).status != 0)
    {

        Rectangle ghost = {(*enemy).box.x, (*enemy).box.y, (*enemy).box.width, (*enemy).box.height};

        float posx= (*personagem).x;
        float posy= (*personagem).y;

        if( (*enemy).box.x > posx)
        {
            ghost.x-=VEL_PERSONAGEM/2.75;
            if(!Colisao(&ghost,map,n_muros))
                (*enemy).box.x -= VEL_PERSONAGEM/2.75;
        }
        if((*enemy).box.x < posx)
        {
            ghost.x+=VEL_PERSONAGEM/2.75;
            if(!Colisao(&ghost,map,n_muros))
                (*enemy).box.x += VEL_PERSONAGEM/2.75;
        }
        if((*enemy).box.y > posy)
        {
            ghost.y-=VEL_PERSONAGEM/2.75;
            if(!Colisao(&ghost,map,n_muros))
                (*enemy).box.y -= VEL_PERSONAGEM/2.75;
        }
        if((*enemy).box.y < posy)
        {
            ghost.y+=VEL_PERSONAGEM/2.75;
            if(!Colisao(&ghost,map,n_muros))
                (*enemy).box.y += VEL_PERSONAGEM/2.75;
        }
    }
}

// MORREU: TESTAR SE O PERSONAGEM MORREU
void Morreu (JOGADOR *player, ENEMY inimigos[], short *pause)
{
    int enemy=0;

    do
    {
       if(CheckCollisionRecs(inimigos[enemy].box,(*player).personagem))
        {
            //(*player).vidas --;
            *pause=1;

        }
        enemy++;
    }while(enemy<N_INIMIGOS && *pause==0);
    //if((*player).vidas == 0)
    //    *pause=1;

}

// REINICIAR FASE:

void ReiniciarFase(JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause, int *sorteado, int *num_inimigo)
{
    int i;
    *pause=0;
    (*player).personagem.x = (SCREENWIDTH-LADO)/2;
    (*player).personagem.y = (SCREENHEIGHT-LADO)/2;
    for(i=0;i<*acm_bomba;i++)
    {
        (*player).bombs[i].posicao.x=-LADO;
        (*player).bombs[i].posicao.y=-LADO;
        (*player).bombs[i].stats=guardada;
    }
    *acm_bomba=0;
    SpawnInimigo(inimigos,map,num_inimigo);
}

// COLAR BOMBA NO MAPA

void BotarBomba (JOGADOR *player, short *acm_bomba,short *bombas_mais)
{
    if(IsKeyPressed(KEY_SPACE) && *acm_bomba<N_BOMBAS + *bombas_mais)
    {
        (*player).bombs[*acm_bomba].posicao.x = ((int) ((*player).personagem.x + LADO/2)/LADO)*LADO; // CALCUCLO PARA DEIXAR NA GRADE DE BLOCOS
        (*player).bombs[*acm_bomba].posicao.y = ((int) ((*player).personagem.y + LADO/2)/LADO)*LADO; // CALCUCLO PARA DEIXAR NA GRADE DE BLOCOS
        (*player).bombs[*acm_bomba].posicao.height = LADO/1.5;
        (*player).bombs[*acm_bomba].posicao.width = LADO/1.5;
        (*player).bombs[*acm_bomba].stats = posicionada;
        (*acm_bomba)++;
    }
}

// DETONAR: EXPLODIR AS BOMBAS POSICIONADAS

void Detonar (JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause,int* n_muros)
{
    short i;

    BOMBAS *bomb;
    for(i=0;i<*acm_bomba;i++)
    {
        bomb= &(*player).bombs[i];
        Explosao(bomb,player,map,inimigos,acm_bomba,pause,n_muros);
    }

    *acm_bomba=0;
}

// EXPLOSAO: VERIFICAR RAIO DA BOMBA

void Explosao (BOMBAS *bomba, JOGADOR *player, MAPA *map, ENEMY inimigos[], short *acm_bomba, short *pause,int *n_muros)
{
    float posX = (*bomba).posicao.x, posY = (*bomba).posicao.y;
    short fogo[BARREIRA]={0};
    int i;

    Fire(bomba,player,map,inimigos,pause,n_muros);
    for(i=1;i<=RAIO;i++)
    {
        (*bomba).posicao.x = posX;
        (*bomba).posicao.y = posY;
        if(fogo[0]==0) //EXPLOSAO PARA CIMA
        {
            (*bomba).posicao.y-=i*LADO;
            DrawRectangleRec((*bomba).posicao,PINK);
            fogo[0]=Fire(bomba,player,map,inimigos,pause,n_muros);
            (*bomba).posicao.y+=i*LADO;
        }
        if(fogo[1]==0) //EXPLOSAO PARA DIREITA
        {
            (*bomba).posicao.x+=i*LADO;
            DrawRectangleRec((*bomba).posicao,PINK);
            fogo[1]=Fire(bomba,player,map,inimigos,pause,n_muros);
            (*bomba).posicao.x-=i*LADO;
        }

        if(fogo[BARREIRA-2]==0) //EXPLOSAO PARA BAIXO
        {
            (*bomba).posicao.y+=i*LADO;
            DrawRectangleRec((*bomba).posicao,PINK);
            fogo[BARREIRA-2]=Fire(bomba,player,map,inimigos,pause,n_muros);
            (*bomba).posicao.y-=i*LADO;
        }
        if(fogo[BARREIRA-1]==0)  //EXPLOSAO PARA ESQUERDA
        {
           (*bomba).posicao.x-=i*LADO;
           DrawRectangleRec((*bomba).posicao,PINK);
            fogo[BARREIRA-1]=Fire(bomba,player,map,inimigos,pause,n_muros);
        }
    }
    (*bomba).posicao.x=-LADO;
    (*bomba).posicao.y=-LADO;
    (*bomba).stats=guardada;
}

// FIRE: RAIO DO FOGO DA EXPLOSAO DA BOMBA CHECANDO COLISÃO COM OS OBJETOS

short Fire (BOMBAS *bomba, JOGADOR *player, MAPA *map, ENEMY inimigos[], short *pause,int*n_muros)
{
    if(CheckCollisionRecs((*bomba).posicao,(*player).personagem))
        *pause=1;
    else
    {
        MatarEnemy(bomba,inimigos);
        if(PercorrerLimite(bomba,(*map).limites) || PercorrerFixa(bomba,(*map).paredesFixas) || PercorrerMuro(bomba,(*map).tijolos,n_muros) )
            return 1; // NÃO FAZER MAIS NESSE SENTIDO
    }
    return 0;
}

// MATAR INIMIGOS COM A BOMBA:

void MatarEnemy(BOMBAS *bomba, ENEMY inimigos[])
{
    int i;

    for(i=0;i<N_INIMIGOS;i++)
       if(CheckCollisionRecs((*bomba).posicao,inimigos[i].box))
       {
           inimigos[i].box.x=-LADO;
           inimigos[i].box.y=-LADO;
           inimigos[i].status=0;
       }
}

// VERIFICAR LIMITE COM BOMBA:

short PercorrerLimite(BOMBAS *bomba, Rectangle limites[])
{
    int i=0;
    do
    {
        if(CheckCollisionRecs((*bomba).posicao,limites[i]))
            return 1;
        i++;
    }while(i<BARREIRA);

    return 0;
}

// VERIFICAR PAREDES FIXA COM BOMBA:

short PercorrerFixa(BOMBAS *bomba, Rectangle paredesFixas[])
{
    int i=0;
    do
    {
        if(CheckCollisionRecs((*bomba).posicao,paredesFixas[i]))
            return 1;
        i++;
    }while(i<N_PAREDES);

    return 0;
}

// VERIFICAR MUROS COM BOMBA:

short PercorrerMuro(BOMBAS *bomba, MURO tijolos[],int *n_muros)
{
    int i=0;
    do
    {
        if(CheckCollisionRecs((*bomba).posicao,tijolos[i].formato))
        {
            tijolos[i].status=destruido;
            tijolos[i].formato.x=-LADO;
            tijolos[i].formato.y=-LADO;
            return 1;
        }
        i++;
    }while(i<*n_muros);

    return 0;
}

// BACKUP: DO ULTIMO JOGO

short Backup(DADOS players[])
{
    FILE *arquivo, *backup;
    int i=0;


    arquivo=fopen("RANKING.bin","rb");
    if(!arquivo)
        printf("Erro na abertura do arquivo de leitura!\n\n");
    else
    {
        printf("Arquivo de leitura aberto\n\n");
        backup=fopen("RANKING_BACKUP.bin","wb");
        if(!backup)
            printf("Erro na abertura do arquivo de backup!\n\n");
        else
        {
            i=0;
            while(i<N_RANKING && !feof(arquivo) && (fread(&players[i],sizeof(DADOS),1,arquivo) == 1))
            {
                   fwrite(&players[i],sizeof(DADOS),1,backup);
                   i++;
            }
            printf("\n\nBACKUP REALIZADO\n");
            fclose(backup);
        }
        fclose(arquivo);
    }
    return i; // Quantos jogadores estao no ranking
}

//  PEGAR AS INFORMACOES PARA CLASSIFICAR NO RANKING

void Classificar(DADOS *pessoa, short fase)
{
    int opcao;

    system("cls");
    do
    {
        fflush(stdin);
        printf("Informe nome para o ranking:\n");
        gets((*pessoa).nome);
        printf("%s, Confirma? (1 - SIM / 0 - Nao):\n", (*pessoa).nome);
        scanf("%d",&opcao);
    }while(opcao!=1);
}

// ORDENAR RANKING

void Ordenar(DADOS players[], short *jogadas)
{
    DADOS aux;
    int i;


    for(i=0;i<*jogadas;i++)
    {
        if(players[i+1].pontuacao >= players[i].pontuacao)
        {

            strcpy(aux.nome,players[i].nome);
            aux.pontuacao=players[i].pontuacao;
            aux.tempo=players[i].tempo;

            //Maior => posicao menor (primeiro da lista)
            strcpy(players[i].nome,players[i+1].nome);
            players[i].pontuacao=players[i+1].pontuacao;
            players[i].tempo=players[i+1].tempo;

            //posicao copia = novo
            strcpy(players[i+1].nome,aux.nome);
            players[i+1].pontuacao=aux.pontuacao;
            players[i+1].tempo=aux.tempo;
        }
    }
}

// LISTAR RANKING

void Ranking(DADOS players[], short *jogadas)
{
    short i;

    system("cls");
    for(i=0;i<*jogadas;i++)
    {
        printf("COLOCACAO: %d\n",i+1);
        puts(players[i].nome);
        printf("PONTUACAO: %.2f\nTEMPO: %lf\n\n",players[i].pontuacao, players[i].tempo);
    }
}

// GERAR RANKING.BIN NOVO

void GerarRanking(DADOS players[], short *jogadas)
{
    FILE *arquivo;
    int i=0;

    arquivo=fopen("RANKING.bin","wb");
    if(!arquivo)
        printf("\n\nErro ao abrir o arquivo para gerar RANKING!");
    else
    {
        do
        {
            fwrite(&players[i],sizeof(DADOS),1,arquivo);
            i++;
        }while(i<*jogadas);

        fclose(arquivo);
        printf("\n\nRANKING GERADO!");
    }
}
