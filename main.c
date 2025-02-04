// INTERFACE DO USUARIO E MENU PRINCIPAL

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.h"

int main()
{
    int opcao;

    do
    {
        printf("\n=== Sistema de Monitoramento de Energia ===\n");
        printf("1. Registrar nova medicao\n");
        printf("2. Calcular medias de consumo\n");
        printf("3. Identificar picos de consumo\n");
        printf("4. Exibir consumo e custo por periodo\n");
        printf("5. Analisar padroes de uso\n");
        printf("6. Gerar relatorio detalhado\n");
        printf("7. Editar medicao\n");
        printf("8. Excluir medicao\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
        {
            Medicao med;
            printf("Digite o consumo em kWh: ");
            scanf("%f", &med.consumo_kwh);
            printf("Digite a tarifa atual: ");
            scanf("%f", &med.tarifa);
            med.timestamp = time(NULL);
            salvarMedicao(med);
            printf("Medicao registrada com sucesso!\n");
            break;
        }
        case 2:
        {
            printf("\nEscolha o periodo:\n");
            printf("1. Media diaria\n");
            printf("2. Media semanal\n");
            printf("3. Media mensal\n");
            int periodo;
            scanf("%d", &periodo);
            switch (periodo)
            {
            case 1:
                printf("Media diaria: %.2f kWh\n", calcularMediaPeriodo(1));
                break;
            case 2:
                printf("Media semanal: %.2f kWh\n", calcularMediaPeriodo(7));
                break;
            case 3:
                printf("Media mensal: %.2f kWh\n", calcularMediaPeriodo(30));
                break;
            default:
                printf("Opcao invalida!\n");
            }
            break;
        }
        case 3:
            identificarPicosConsumo();
            break;
        case 4:
        {
            printf("\nDigite o periodo em dias para analise: ");
            int dias;
            scanf("%d", &dias);
            if (dias > 0)
            {
                calcularConsumoPeriodo(dias);
            }
            else
            {
                printf("Periodo invalido!\n");
            }
            break;
        }
        case 5:
            analisarPadroes();
            break;
        case 6:
            gerarRelatorio();
            break;
        case 7:
        {
            listarMedicoes();
            printf("\nDigite o indice da medicao que deseja editar: ");
            int indice;
            scanf("%d", &indice);

            Medicao novaMedicao;
            printf("Digite o novo consumo em kWh: ");
            scanf("%f", &novaMedicao.consumo_kwh);
            printf("Digite a nova tarifa: ");
            scanf("%f", &novaMedicao.tarifa);

            editarMedicao(indice, novaMedicao);
            break;
        }
        case 8:
        {
            listarMedicoes();
            printf("\nDigite o indice da medicao que deseja excluir: ");
            int indice;
            scanf("%d", &indice);

            printf("Tem certeza que deseja excluir esta medicao? (1-Sim/0-Nao): ");
            int confirma;
            scanf("%d", &confirma);

            if (confirma == 1)
            {
                excluirMedicao(indice);
            }
            else
            {
                printf("Operacao cancelada!\n");
            }
            break;
        }
        case 0:
            printf("Encerrando o programa...\n");
            break;
        default:
            printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}