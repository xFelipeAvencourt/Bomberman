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
