// DECLARAÇÕES DE ESTRUTURAS E FUNÇÕES
#ifndef FUNCOES_H
#define FUNCOES_H
#include <float.h>

typedef struct
{
    time_t timestamp;  
    float consumo_kwh; 
    float tarifa;     
} Medicao;

void salvarMedicao(Medicao med);
float calcularMediaPeriodo(int dias);
void identificarPicosConsumo(void);
void calcularConsumoPeriodo(int dias);
void analisarPadroes(void);
void gerarRelatorio(void);
void editarMedicao(int indice, Medicao novaMedicao);
void excluirMedicao(int indice);
void listarMedicoes(void);
float calcularMediaRecursiva(Medicao *medicoes, int inicio, int fim);
Medicao *carregarMedicoes(int *total);

#endif