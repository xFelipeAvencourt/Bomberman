🧨 Bomberman – Projeto em C (Raylib)

Projeto desenvolvido por Felipe Avencourt e Vicente Isatto, minha dupla do projeto final da cadeira Introdução a Algoritmos e Programação.
O jogo é uma recriação do clássico Bomberman, escrito inteiramente em C99 e utilizando a biblioteca gráfica Raylib.

Sobre o Projeto

Este jogo implementa:

Movimento do personagem em um mapa baseado em grid

Colocação e explosão de bombas

Destruição de blocos

Inimigos com movimento simples

Contagem de fases

Sistema completo de ranking (salvo em arquivo)

Sprites animados

Colisões e sistema básico de física

Interface gráfica com Raylib

Toda a lógica principal do jogo está definida em bomber.h e implementada em Bomber.c, enquanto main.c controla o fluxo do jogo, ranking e inicialização.

🚀 Como Compilar
Pré-requisitos

Compiler C (GCC ou Clang)

Raylib instalada

No Linux (Ubuntu):

sudo apt install gcc make
sudo apt install libraylib-dev

Compilação
gcc main.c Bomber.c -lraylib -lm -o bomberman


Se sua instalação do Raylib estiver em um diretório específico, pode ser necessário adicionar:

-I /usr/include -L /usr/lib

🎮 Como Jogar

Setas / WASD – mover personagem

Barra de espaço – plantar bomba

Objetivo: destruir blocos, evitar inimigos e completar as fases.
