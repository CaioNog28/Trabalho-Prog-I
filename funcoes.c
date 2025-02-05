// IMPLEMENTAÇÃO DE FUNÇÕES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "funcoes.h"
#include <float.h>

#define ARQUIVO_MEDICOES "medicoes.csv"
#define ARQUIVO_RELATORIO "relatorio.txt"
#define ARQUIVO_SUGESTOES "sugestoes.txt"
#define ARQUIVO_PICOS "picos.txt"

void salvarMedicao(Medicao med)
{
    FILE *fp;
    int arquivo_existe = 0;

    if ((fp = fopen(ARQUIVO_MEDICOES, "r")))
    {
        arquivo_existe = 1;
        fclose(fp);
    }

    fp = fopen(ARQUIVO_MEDICOES, "a");
    if (fp == NULL)
    {
        printf("Erro ao abrir arquivo de medicoes!\n");
        return;
    }

    if (!arquivo_existe)
    {
        fprintf(fp, "Data,Consumo (kWh),Tarifa (R$)\n");
    }

    char data[26];
    struct tm *timeinfo = localtime(&med.timestamp);
    strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", timeinfo);

    fprintf(fp, "%s,%.2f,%.2f\n", data, med.consumo_kwh, med.tarifa);

    fclose(fp);
}

Medicao *carregarMedicoes(int *total)
{
    FILE *fp = fopen(ARQUIVO_MEDICOES, "r");
    if (fp == NULL)
    {
        printf("Erro ao abrir arquivo de medicoes!\n");
        *total = 0;
        return NULL;
    }

    Medicao *medicoes = NULL;
    *total = 0;
    char linha[256];
    char data[26];
    float consumo, tarifa;

    if (!fgets(linha, sizeof(linha), fp))
    {
        printf("Erro ao ler cabeçalho do arquivo!\n");
        fclose(fp);
        return NULL;
    }

    while (fgets(linha, sizeof(linha), fp))
    {
        struct tm tm_data = {0};

        if (sscanf(linha, "%d/%d/%d %d:%d:%d,%f,%f",
                   &tm_data.tm_mday, &tm_data.tm_mon, &tm_data.tm_year,
                   &tm_data.tm_hour, &tm_data.tm_min, &tm_data.tm_sec,
                   &consumo, &tarifa) == 8)
        {

            tm_data.tm_mon -= 1;
            tm_data.tm_year -= 1900;

            medicoes = realloc(medicoes, (*total + 1) * sizeof(Medicao));
            if (medicoes == NULL)
            {
                printf("Erro de alocação de memória!\n");
                fclose(fp);
                return NULL;
            }

            medicoes[*total].timestamp = mktime(&tm_data);
            medicoes[*total].consumo_kwh = consumo;
            medicoes[*total].tarifa = tarifa;
            (*total)++;
        }
    }

    fclose(fp);
    return medicoes;
}
float calcularMediaPeriodo(int dias)
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        return 0.0;
    }

    time_t limite = time(NULL) - (dias * 24 * 60 * 60);
    int contPeriodo = 0;
    float somaConsumo = 0.0;

    for (int i = 0; i < total; i++)
    {
        if (difftime(medicoes[i].timestamp, limite) >= 0)
        {
            somaConsumo += medicoes[i].consumo_kwh;
            contPeriodo++;
        }
    }

    free(medicoes);
    return (contPeriodo > 0) ? (somaConsumo / contPeriodo) : 0.0;
}
float calcularMediaRecursiva(Medicao *medicoes, int inicio, int fim)
{
    if (inicio == fim)
    {
        return medicoes[inicio].consumo_kwh;
    }

    int meio = (inicio + fim) / 2;

    float mediaEsquerda = calcularMediaRecursiva(medicoes, inicio, meio);
    float mediaDireita = calcularMediaRecursiva(medicoes, meio + 1, fim);

    return ((float)(meio - inicio + 1) * mediaEsquerda +
            (float)(fim - meio) * mediaDireita) /
           (fim - inicio + 1);
}

void identificarPicosConsumo()
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        return;
    }

    float media = calcularMediaRecursiva(medicoes, 0, total - 1);
    float limiar = media * 1.5;

    FILE *fpPicos = fopen(ARQUIVO_PICOS, "w");
    if (fpPicos == NULL)
    {
        printf("Erro ao criar arquivo de picos!\n");
        free(medicoes);
        return;
    }

    fprintf(fpPicos, "=== PICOS DE CONSUMO ===\n");
    fprintf(fpPicos, "Media de consumo: %.2f kWh\n", media);
    fprintf(fpPicos, "Limiar considerado: %.2f kWh\n\n", limiar);

    for (int i = 0; i < total; i++)
    {
        if (medicoes[i].consumo_kwh > limiar)
        {
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

void calcularConsumoPeriodo(int dias)
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        return;
    }

    time_t limite = time(NULL) - (dias * 24 * 60 * 60);
    float consumoTotal = 0.0;
    float custoTotal = 0.0;
    int medicoesPeriodo = 0;

    for (int i = 0; i < total; i++)
    {
        if (medicoes[i].timestamp >= limite)
        {
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

void listarMedicoes()
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        printf("Nenhuma medicao encontrada!\n");
        return;
    }

    printf("\n=== Medicoes Registradas ===\n");
    for (int i = 0; i < total; i++)
    {
        char data[26];
        strcpy(data, ctime(&medicoes[i].timestamp));
        data[24] = '\0';
        printf("\nIndice: %d\n", i + 1);
        printf("Data: %s\n", data);
        printf("Consumo: %.2f kWh\n", medicoes[i].consumo_kwh);
        printf("Tarifa: R$ %.2f\n", medicoes[i].tarifa);
    }

    free(medicoes);
}

void editarMedicao(int indice, Medicao novaMedicao)
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        printf("Nenhuma medicao encontrada!\n");
        return;
    }

    if (indice < 1 || indice > total)
    {
        printf("Indice invalido!\n");
        free(medicoes);
        return;
    }

    novaMedicao.timestamp = medicoes[indice - 1].timestamp;
    medicoes[indice - 1] = novaMedicao;

    FILE *fp = fopen(ARQUIVO_MEDICOES, "w");
    if (fp == NULL)
    {
        printf("Erro ao abrir arquivo para edicao!\n");
        free(medicoes);
        return;
    }
    fprintf(fp, "Data,Consumo (kWh),Tarifa (R$)\n");

    for (int i = 0; i < total; i++)
    {
        char data[26];
        struct tm *timeinfo = localtime(&medicoes[i].timestamp);
        strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", timeinfo);
        fprintf(fp, "%s,%.2f,%.2f\n",
                data,
                medicoes[i].consumo_kwh,
                medicoes[i].tarifa);
    }

    fclose(fp);
    free(medicoes);
    printf("Medicao editada com sucesso!\n");
}
void excluirMedicao(int indice)
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        printf("Nenhuma medicao encontrada!\n");
        return;
    }

    if (indice < 1 || indice > total)
    {
        printf("Indice invalido!\n");
        free(medicoes);
        return;
    }

    for (int i = indice - 1; i < total - 1; i++)
    {
        medicoes[i] = medicoes[i + 1];
    }

    FILE *fp = fopen(ARQUIVO_MEDICOES, "w");
    if (fp == NULL)
    {
        printf("Erro ao abrir arquivo para exclusao!\n");
        free(medicoes);
        return;
    }

    fprintf(fp, "Data,Consumo (kWh),Tarifa (R$)\n");

    for (int i = 0; i < total - 1; i++)
    {
        char data[26];
        struct tm *timeinfo = localtime(&medicoes[i].timestamp);
        strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", timeinfo);
        fprintf(fp, "%s,%.2f,%.2f\n",
                data,
                medicoes[i].consumo_kwh,
                medicoes[i].tarifa);
    }

    fclose(fp);
    free(medicoes);
    printf("Medicao excluida com sucesso!\n");
}
void analisarPadroes()
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        return;
    }

    float consumoHora[24] = {0};
    int contadorHora[24] = {0};

    for (int i = 0; i < total; i++)
    {
        struct tm *timeinfo = localtime(&medicoes[i].timestamp);
        consumoHora[timeinfo->tm_hour] += medicoes[i].consumo_kwh;
        contadorHora[timeinfo->tm_hour]++;
    }

    FILE *fpSugestoes = fopen(ARQUIVO_SUGESTOES, "w");
    if (fpSugestoes == NULL)
    {
        printf("Erro ao criar arquivo de sugestoes!\n");
        free(medicoes);
        return;
    }

    fprintf(fpSugestoes, "=== ANÁLISE DE PADRÕES DE CONSUMO ===\n\n");
    fprintf(fpSugestoes, "Consumo médio por hora do dia:\n\n");

    float maiorMedia = 0;
    float menorMedia = FLT_MAX;
    int horaPico = 0;
    int horaVale = 0;

    for (int hora = 0; hora < 24; hora++)
    {
        if (contadorHora[hora] > 0)
        {
            float media = consumoHora[hora] / contadorHora[hora];
            fprintf(fpSugestoes, "%02d:00 - %.2f kWh\n", hora, media);

            if (media > maiorMedia)
            {
                maiorMedia = media;
                horaPico = hora;
            }
            if (media < menorMedia && contadorHora[hora] > 0)
            {
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

void gerarRelatorio()
{
    int total;
    Medicao *medicoes = carregarMedicoes(&total);
    if (medicoes == NULL || total == 0)
    {
        return;
    }

    FILE *fpRelatorio = fopen(ARQUIVO_RELATORIO, "w");
    if (fpRelatorio == NULL)
    {
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
    for (int i = 0; i < total; i++)
    {
        char data[26];
        struct tm *timeinfo = localtime(&medicoes[i].timestamp);
        strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", timeinfo);
        fprintf(fpRelatorio, "Data: %s\n", data);
        fprintf(fpRelatorio, "Consumo: %.2f kWh\n", medicoes[i].consumo_kwh);
        fprintf(fpRelatorio, "Tarifa: R$ %.2f\n", medicoes[i].tarifa);
        fprintf(fpRelatorio, "Custo: R$ %.2f\n\n", medicoes[i].consumo_kwh * medicoes[i].tarifa);
    }

    fclose(fpRelatorio);
    free(medicoes);
    printf("Relatório gerado com sucesso! Verifique o arquivo %s\n", ARQUIVO_RELATORIO);
}