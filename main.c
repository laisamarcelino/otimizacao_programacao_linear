#include <stdio.h>
#include "problem.h"

int main(void) {
    Instancia inst;

    /* Lê a instância de stdin e gera o modelo LP em stdout. */
    if (!ler_instancia(stdin, &inst)) {
        fprintf(stderr, "Erro ao ler a entrada.\n");
        return 1;
    }

    gerar_lp(stdout, &inst);
    liberar_instancia(&inst);

    return 0;
}
