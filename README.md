# Trabalho-Prog-I

Estrutura do Projeto
O sistema foi estruturado em três arquivos principais que trabalham em conjunto:

funcoes.h: Arquivo de cabeçalho com as declarações
funcoes.c: Implementação das funções
main.c: Chamada das funções de acordo com a opção do usuário

Fase 1: Definição da Estrutura de Dados
Inicialmente, foi definida a estrutura fundamental Medicao no arquivo funcoes.h

Esta estrutura foi escolhida para armazenar os dados essenciais de cada medição de energia, permitindo o registro temporal do consumo e seu custo associado.
Fase 2: Desenvolvimento do Sistema de Armazenamento
O sistema utiliza arquivos binários para persistência dos dados, implementando as seguintes funções principais:

salvarMedicao: Responsável por armazenar cada nova medição no arquivo "medicoes.dat"
carregarMedicoes: Recupera todas as medições armazenadas em memória para processamento

O armazenamento binário foi escolhido para garantir eficiência e integridade dos dados.
Fase 3: Implementação das Análises
Foram desenvolvidos vários tipos de análises para processar os dados coletados:
3.1 Análise de Médias
A função calcularMediaRecursiva implementa um algoritmo de divisão e conquista para calcular médias de consumo. Esta abordagem foi escolhida para demonstrar eficiência com grandes volumes de dados.
3.2 Identificação de Picos
A função identificarPicosConsumo analisa:

Calcula a média geral de consumo
Identifica e registra consumos acima da média

3.3 Análise de Padrões
analisarPadroes implementa:

Agrupamento de consumo por hora do dia
Identificação de horários de pico e vale
Geração de recomendações personalizadas

Fase 4: Sistema de Relatórios
Foram implementados três tipos de relatórios:

Relatório Geral (gerarRelatorio):

Médias de consumo (diária, semanal, mensal)
Histórico completo de medições
Custos associados


Relatório de Picos (identificarPicosConsumo):

Registro de consumos anormais
Análise de excesso em relação à média


Relatório de Sugestões (analisarPadroes):

Análise temporal do consumo
Recomendações para economia



Fase 5: Interface do Usuário
O arquivo main.c implementa uma interface em linha de comando com menu interativo, oferecendo:

Registro de novas medições
Cálculo de médias por período
Identificação de picos
Análise de consumo por período
Análise de padrões
Geração de relatórios

Aspectos Técnicos Relevantes
Gerenciamento de Memória

Utilização de alocação dinâmica com malloc/realloc
Liberação adequada de memória com free
Tratamento de erros em operações com arquivos

Modularização

Separação clara entre interface (main.c) e lógica de negócio (funcoes.c)
Encapsulamento de funcionalidades através do header (funcoes.h)
Reutilização de código através de funções auxiliares

Tratamento de Dados Temporais

Uso da biblioteca time.h para manipulação de datas
Conversão adequada entre formatos temporais
Cálculos precisos de períodos

Possibilidades de Expansão
O sistema foi projetado pensando em futuras expansões, como:

Interface gráfica
Exportação de dados para outros formatos
Análises estatísticas mais complexas
Integração com medidores automáticos
Sistema de alertas em tempo real

Conclusão
O Sistema de Monitoramento de Energia demonstra uma implementação robusta e bem estruturada, com foco em:

Modularidade e manutenibilidade
Eficiência no processamento e armazenamento
Análises úteis para o usuário final
Geração de relatórios informativos
Possibilidade de expansão futura

Este sistema serve como base sólida para um monitoramento efetivo do consumo de energia, permitindo aos usuários tomar decisões informadas sobre seu consumo energético.

O código é bem modularizado e contém implementações recursivas em funções separadas.

Diferenças entre versões
Na primeira versão do código optamos por fazer tudo dentro de um só arquivo, e apesar de funcional, era de difícil interpretração, a separação dos arquivos fez toda a diferença para a otimização e entendimento do código.

O código também apresentava diferença na hora de salvar os dados brutos. Na primeira versão, optamos por salvar os dados em um arquivo .dat, o que fazia com que fosse feito em binário, nao sendo legível para o usuário. Já na última versão alteramos para que fosse criado um arquivo .csv para armazenar os dados brutos, assim sendo legível para o usuário quando o mesmo achar ser necessário.


Fontes
Claude.ai
chatpgt.com
youtube.com/@DeAlunoParaAluno
clubedohardware.com.br
