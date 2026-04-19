#include "problem.h"

#include <stdlib.h>

static void zerar_instancia(Instancia *inst) {
    if (inst == NULL) return;

    inst->c = 0;
    inst->p = 0;
    inst->k = 0;
    inst->q = NULL;
    inst->comps = NULL;
    inst->restr = NULL;
}

static void limpar_instancia(Instancia *inst) {
    int i;

    if (inst == NULL) return;

    if (inst->comps != NULL) {
        for (i = 0; i < inst->p; i++) {
            free(inst->comps[i].comp);
            inst->comps[i].comp = NULL;
        }
    }

    free(inst->comps);
    free(inst->q);
    free(inst->restr);

    inst->comps = NULL;
    inst->q = NULL;
    inst->restr = NULL;
    inst->c = 0;
    inst->p = 0;
    inst->k = 0;
}

static int ler_vetor(FILE *in, int *valores, int quantidade) {
    int i;

    for (i = 0; i < quantidade; i++) {
        if (fscanf(in, "%d", &valores[i]) != 1) {
            return 0;
        }
    }

    return 1;
}

static int ler_comprimidos(FILE *in, Instancia *inst) {
    int i;

    for (i = 0; i < inst->p; i++) {
        inst->comps[i].comp = (int *) malloc((size_t) inst->c * sizeof(int));
        if (inst->comps[i].comp == NULL) {
            return 0;
        }

        if (!ler_vetor(in, inst->comps[i].comp, inst->c)) {
            return 0;
        }

        if (fscanf(in, "%d", &inst->comps[i].preco) != 1) {
            return 0;
        }
    }

    return 1;
}

static int ler_restricoes(FILE *in, Instancia *inst) {
    int i;

    if (fscanf(in, "%d", &inst->k) != 1) {
        return 0;
    }

    if (inst->k < 0) {
        return 0;
    }

    if (inst->k == 0) {
        return 1;
    }

    inst->restr = (Restricao *) malloc((size_t) inst->k * sizeof(Restricao));
    if (inst->restr == NULL) {
        return 0;
    }

    for (i = 0; i < inst->k; i++) {
        if (fscanf(in, "%d %d", &inst->restr[i].componente, &inst->restr[i].limite) != 2) {
            return 0;
        }

        if (inst->restr[i].componente < 1 || inst->restr[i].componente > inst->c) {
            return 0;
        }
    }

    return 1;
}

/* Mantém a sintaxe do lp_solve consistente para sinais positivos e negativos. */
static void imprimir_termo(FILE *out, int coef, int var_index, int primeiro) {
    if (out == NULL) return;

    if (primeiro) {
        fprintf(out, "%d x%d", coef, var_index);
    } else if (coef >= 0) {
        fprintf(out, " + %d x%d", coef, var_index);
    } else {
        fprintf(out, " - %d x%d", -coef, var_index);
    }
}

static void imprimir_objetivo(FILE *out, const Instancia *inst) {
    int i;
    int primeiro = 1;

    fprintf(out, "min: ");

    for (i = 0; i < inst->p; i++) {
        int preco = inst->comps[i].preco;

        if (preco == 0) {
            continue;
        }

        imprimir_termo(out, preco, i + 1, primeiro);
        primeiro = 0;
    }

    if (primeiro) {
        fprintf(out, "0");
    }

    fprintf(out, ";\n\n");
}

static void imprimir_restricoes_minimas(FILE *out, const Instancia *inst) {
    int i;
    int j;

    /* Cada componente deve atingir a quantidade diária mínima exigida. */
    for (j = 0; j < inst->c; j++) {
        int primeiro = 1;

        for (i = 0; i < inst->p; i++) {
            int coef = inst->comps[i].comp[j];

            if (coef == 0) {
                continue;
            }

            imprimir_termo(out, coef, i + 1, primeiro);
            primeiro = 0;
        }

        if (primeiro) {
            fprintf(out, "0");
        }

        fprintf(out, " >= %d;\n", inst->q[j]);
    }

    fprintf(out, "\n");
}

static void imprimir_restricoes_limite(FILE *out, const Instancia *inst) {
    int i;
    int t;

    for (t = 0; t < inst->k; t++) {
        int comp_idx = inst->restr[t].componente - 1;
        int primeiro = 1;

        for (i = 0; i < inst->p; i++) {
            int coef = inst->comps[i].comp[comp_idx];

            if (coef == 0) {
                continue;
            }

            imprimir_termo(out, coef, i + 1, primeiro);
            primeiro = 0;
        }

        if (primeiro) {
            fprintf(out, "0");
        }

        fprintf(out, " <= %d;\n", inst->restr[t].limite);
    }

    if (inst->k > 0) {
        fprintf(out, "\n");
    }
}

static void imprimir_dominio(FILE *out, const Instancia *inst) {
    int i;

    /* A quantidade de comprimidos por dia não pode ser negativa. */
    for (i = 0; i < inst->p; i++) {
        fprintf(out, "x%d >= 0;\n", i + 1);
    }

    /* O enunciado pede quantidade inteira de comprimidos. */
    fprintf(out, "\nint ");

    for (i = 0; i < inst->p; i++) {
        if (i > 0) {
            fprintf(out, ", ");
        }
        fprintf(out, "x%d", i + 1);
    }

    fprintf(out, ";\n");
}

int ler_instancia(FILE *in, Instancia *inst) {
    if (in == NULL || inst == NULL) {
        return 0;
    }

    zerar_instancia(inst);

    /* Lê c componentes e p tipos de comprimidos. */
    if (fscanf(in, "%d %d", &inst->c, &inst->p) != 2) {
        limpar_instancia(inst);
        return 0;
    }

    if (inst->c <= 0 || inst->p <= 0) {
        limpar_instancia(inst);
        return 0;
    }

    inst->q = (int *) malloc((size_t) inst->c * sizeof(int));
    inst->comps = (Comprimido *) calloc((size_t) inst->p, sizeof(Comprimido));

    if (inst->q == NULL || inst->comps == NULL) {
        limpar_instancia(inst);
        return 0;
    }

    /* q[j] é a quantidade diária mínima do componente j. */
    if (!ler_vetor(in, inst->q, inst->c)) {
        limpar_instancia(inst);
        return 0;
    }

    /* Cada comprimido informa c quantidades de componentes e um preço. */
    if (!ler_comprimidos(in, inst)) {
        limpar_instancia(inst);
        return 0;
    }

    /* Cada restrição limitada informa um componente fi e um limite li. */
    if (!ler_restricoes(in, inst)) {
        limpar_instancia(inst);
        return 0;
    }

    return 1;
}

void gerar_lp(FILE *out, const Instancia *inst) {
    if (out == NULL || inst == NULL) {
        return;
    }

    /* A saída segue a ordem esperada pelo formato do lp_solve. */
    imprimir_objetivo(out, inst);
    imprimir_restricoes_minimas(out, inst);
    imprimir_restricoes_limite(out, inst);
    imprimir_dominio(out, inst);
}

void liberar_instancia(Instancia *inst) {
    limpar_instancia(inst);
}
