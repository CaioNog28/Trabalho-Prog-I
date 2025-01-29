//IMPLEMENTAÇÃO DE FUNÇÕES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "funcoes.h"

#define ARQUIVO_MEDICOES "medicoes.dat"
#define ARQUIVO_RELATORIO "relatorio.txt"
#define ARQUIVO_SUGESTOES "sugestoes.txt"
#define ARQUIVO_PICOS "picos.txt"

void salvarMedicao(Medicao med) {
    FILE* fp = fopen(ARQUIVO_MEDICOES, "ab");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo de medicoes!\n");
        return;
    }
    fwrite(&med, sizeof(Medicao), 1, fp);
    fclose(fp);
}

Medicao* carregarMedicoes(int* total) {
    FILE* fp = fopen(ARQUIVO_MEDICOES, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo de medicoes!\n");
        *total = 0;
        return NULL;
    }
    
    Medicao* medicoes = NULL;
    *total = 0;
    Medicao med;
    
    while (fread(&med, sizeof(Medicao), 1, fp) == 1) {
        medicoes = realloc(medicoes, (*total + 1) * sizeof(Medicao));
        medicoes[*total] = med;
        (*total)++;
    }
    
    fclose(fp);
    return medicoes;
}

float calcularMediaRecursiva(Medicao* medicoes, int inicio, int fim) {
    if (inicio == fim) {
        return medicoes[inicio].consumo_kwh;
    }
    
    int meio = (inicio + fim) / 2;
    float mediaEsquerda = calcularMediaRecursiva(medicoes, inicio, meio);
    float mediaDireita = calcularMediaRecursiva(medicoes, meio + 1, fim);
    
    return (mediaEsquerda + mediaDireita) / 2.0;
}

float calcularMediaPeriodo(int dias) {
    int total;
    Medicao* medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0) {
        return 0.0;
    }
    
    time_t limite = time(NULL) - (dias * 24 * 60 * 60);
    int contPeriodo = 0;
    float somaConsumo = 0.0;
    
    for (int i = 0; i < total; i++) {
        if (medicoes[i].timestamp >= limite) {
            somaConsumo += medicoes[i].consumo_kwh;
            contPeriodo++;
        }
    }
    
    free(medicoes);
    return (contPeriodo > 0) ? (somaConsumo / contPeriodo) : 0.0;
}

void identificarPicosConsumo() {
    int total;
    Medicao* medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0) {
        return;
    }
    
    float media = calcularMediaRecursiva(medicoes, 0, total - 1);
    float limiar = media * 1.5;
    
    FILE* fpPicos = fopen(ARQUIVO_PICOS, "w");
    if (fpPicos == NULL) {
        printf("Erro ao criar arquivo de picos!\n");
        free(medicoes);
        return;
    }
    
    fprintf(fpPicos, "=== PICOS DE CONSUMO ===\n");
    fprintf(fpPicos, "Media de consumo: %.2f kWh\n", media);
    fprintf(fpPicos, "Limiar considerado: %.2f kWh\n\n", limiar);
    
    for (int i = 0; i < total; i++) {
        if (medicoes[i].consumo_kwh > limiar) {
            char data[26];
            strcpy(data, ctime(&medicoes[i].timestamp));
            data[24] = '\0'; 
            fprintf(fpPicos, "Data: %s\n", data);
            fprintf(fpPicos, "Consumo: %.2f kWh\n", medicoes[i].consumo_kwh);
            fprintf(fpPicos, "Excesso: %.2f kWh\n\n", medicoes[i].consumo_kwh - media);
        }
    }
    
    fclose(fpPicos);
    free(medicoes);
    printf("Picos de consumo identificados! Verifique o arquivo %s\n", ARQUIVO_PICOS);
}

void calcularConsumoPeriodo(int dias) {
    int total;
    Medicao* medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0) {
        return;
    }
    
    time_t limite = time(NULL) - (dias * 24 * 60 * 60);
    float consumoTotal = 0.0;
    float custoTotal = 0.0;
    int medicoesPeriodo = 0;
    
    for (int i = 0; i < total; i++) {
        if (medicoes[i].timestamp >= limite) {
            consumoTotal += medicoes[i].consumo_kwh;
            custoTotal += medicoes[i].consumo_kwh * medicoes[i].tarifa;
            medicoesPeriodo++;
        }
    }
    
    printf("\n=== Consumo dos ultimos %d dias ===\n", dias);
    printf("Total de medicoes: %d\n", medicoesPeriodo);
    printf("Consumo total: %.2f kWh\n", consumoTotal);
    printf("Custo total: R$ %.2f\n", custoTotal);
    printf("Media diaria: %.2f kWh\n", consumoTotal / dias);
    printf("Custo medio diario: R$ %.2f\n", custoTotal / dias);
    
    free(medicoes);
}

void analisarPadroes() {
    int total;
    Medicao* medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0) {
        return;
    }
    
    float consumoHora[24] = {0};
    int contadorHora[24] = {0};
    
    for (int i = 0; i < total; i++) {
        struct tm* timeinfo = localtime(&medicoes[i].timestamp);
        consumoHora[timeinfo->tm_hour] += medicoes[i].consumo_kwh;
        contadorHora[timeinfo->tm_hour]++;
    }
    
    FILE* fpSugestoes = fopen(ARQUIVO_SUGESTOES, "w");
    if (fpSugestoes == NULL) {
        printf("Erro ao criar arquivo de sugestoes!\n");
        free(medicoes);
        return;
    }
    
    fprintf(fpSugestoes, "=== ANÁLISE DE PADRÕES DE CONSUMO ===\n\n");
    fprintf(fpSugestoes, "Consumo médio por hora do dia:\n\n");
    
    float maiorMedia = 0;
    float menorMedia = 999999;
    int horaPico = 0;
    int horaVale = 0;
    
    for (int hora = 0; hora < 24; hora++) {
        if (contadorHora[hora] > 0) {
            float media = consumoHora[hora] / contadorHora[hora];
            fprintf(fpSugestoes, "%02d:00 - %.2f kWh\n", hora, media);
            
            if (media > maiorMedia) {
                maiorMedia = media;
                horaPico = hora;
            }
            if (media < menorMedia && contadorHora[hora] > 0) {
                menorMedia = media;
                horaVale = hora;
            }
        }
    }
    
    fprintf(fpSugestoes, "\nSUGESTÕES PARA ECONOMIA:\n");
    fprintf(fpSugestoes, "1. Horário de maior consumo: %02d:00 (%.2f kWh)\n", horaPico, maiorMedia);
    fprintf(fpSugestoes, "2. Horário de menor consumo: %02d:00 (%.2f kWh)\n", horaVale, menorMedia);
    fprintf(fpSugestoes, "\nRecomendações:\n");
    fprintf(fpSugestoes, "- Evite usar aparelhos de alto consumo entre %02d:00 e %02d:00\n", 
            horaPico, (horaPico + 1) % 24);
    fprintf(fpSugestoes, "- Priorize o uso de aparelhos entre %02d:00 e %02d:00\n", 
            horaVale, (horaVale + 1) % 24);
    
    fclose(fpSugestoes);
    free(medicoes);
    printf("Análise de padrões concluída! Verifique o arquivo %s\n", ARQUIVO_SUGESTOES);
}

void gerarRelatorio() {
    int total;
    Medicao* medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0) {
        return;
    }
    
    FILE* fpRelatorio = fopen(ARQUIVO_RELATORIO, "w");
    if (fpRelatorio == NULL) {
        printf("Erro ao criar arquivo de relatorio!\n");
        free(medicoes);
        return;
    }
    
    fprintf(fpRelatorio, "=== RELATÓRIO DETALHADO DE CONSUMO ===\n\n");
    
    fprintf(fpRelatorio, "MÉDIAS DE CONSUMO:\n");
    fprintf(fpRelatorio, "Diária: %.2f kWh\n", calcularMediaPeriodo(1));
    fprintf(fpRelatorio, "Semanal: %.2f kWh\n", calcularMediaPeriodo(7));
    fprintf(fpRelatorio, "Mensal: %.2f kWh\n\n", calcularMediaPeriodo(30));
    
    fprintf(fpRelatorio, "HISTÓRICO DE MEDIÇÕES:\n");
    for (int i = 0; i < total; i++) {
        char data[26];
        strcpy(data, ctime(&medicoes[i].timestamp));
        data[24] = '\0'; 
        fprintf(fpRelatorio, "Data: %s\n", data);
        fprintf(fpRelatorio, "Consumo: %.2f kWh\n", medicoes[i].consumo_kwh);
        fprintf(fpRelatorio, "Tarifa: R$ %.2f\n", medicoes[i].tarifa);
        fprintf(fpRelatorio, "Custo: R$ %.2f\n\n", medicoes[i].consumo_kwh * medicoes[i].tarifa);
    }
    
    fclose(fpRelatorio);
    free(medicoes);
    printf("Relatório gerado com sucesso! Verifique o arquivo %s\n", ARQUIVO_RELATORIO);
}