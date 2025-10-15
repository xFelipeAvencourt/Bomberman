/* DESENVOLVIDO POR FELIPE A. & VICENTE ISATTO */
/*TRABALHO FINAL JOGO EM C99 DA CADEIRA DE INTRODUCAO A ALGORITMOS E PROGRAMACAO */

#include <stdio.h>
#include <raylib.h>
#include "bomber.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

