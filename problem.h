#ifndef PROBLEM_H
#define PROBLEM_H

#include <stdio.h>

typedef struct {
    int *comp;   // vetor de tamanho c
    int preco;   // custo unitário do comprimido
} Comprimido;

typedef struct {
    int componente; // índice 1..c da entrada
    int limite;
} Restricao;

typedef struct {
    int c;                  // quantidade de componentes
    int p;                  // quantidade de tipos de comprimidos
    int k;                  // quantidade de componentes limitados
    int *q;                 // quantidades mínimas diárias, tamanho c
    Comprimido *comps;      // tipos de comprimidos, tamanho p
    Restricao *restr;       // restrições de limite, tamanho k
} Instancia;

int ler_instancia(FILE *in, Instancia *inst);
void gerar_lp(FILE *out, const Instancia *inst);
void liberar_instancia(Instancia *inst);

#endif
