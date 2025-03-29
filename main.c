/* DESENVOLVIDO POR FELIPE A. & VICENTE ISATTO */
/*TRABALHO FINAL JOGO EM C99 DA CADEIRA DE INTRODUCAO A ALGORITMOS E PROGRAMACAO */

#include <stdio.h>
#include <raylib.h>
#include "bomber.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>


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
int main()
{
    DADOS pessoas[N_RANKING+1]; // NUMERO DO RANKING +1, DEVIDO AO NOVO JOGADOR
    short fase=0, jogadas=0;

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Bomberman, por Felipe A. & Vicente isatto");
    SetTargetFPS(60);

    jogadas=Backup(pessoas);
    jogadas++;
    fase=Jogo(&pessoas[jogadas]);

    if(fase)
    {
        Classificar(&pessoas[jogadas],fase);
        Ordenar(pessoas,&jogadas);
        Ranking(pessoas,&jogadas);
        GerarRanking(pessoas,&jogadas);
    }
}
