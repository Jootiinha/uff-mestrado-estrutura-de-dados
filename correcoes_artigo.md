# Correções para alinhar `artigo.tex` à implementação atual

Este arquivo lista trechos do `artigo.tex` que não estão alinhados com o código atual e sugere textos substitutos.

Evidências usadas:

- `benchmark.cpp`: usa tamanhos `15000` a `20000`, `30` vetores por tamanho, `selection_size = 15000` e `selection_iterations = 30`.
- `libraries/block_sort.hpp`: gera blocos de tamanho 4 ou 5, avalia combinações válidas sem repetir algoritmo, executa versões sequencial, `std::thread` e `OpenMP`.
- `plot_results.gp`: gera gráficos a partir de `results/averages.dat`.
- `results/`: está vazio no momento da análise; portanto, o artigo ainda não pode afirmar resultados numéricos finais.

## 1. Metodologia: corrigir a descrição da seleção da melhor combinação

### Problema

Em `artigo.tex`, há dois trechos conflitantes:

- um afirma que a seleção usa as `30` iterações;
- outro, marcado como `REVER`, afirma que usa `2.000` elementos e `5` iterações.

O código atual usa:

```cpp
const int selection_iterations = 30;
const int selection_size = 15000;
```

### Substituir o trecho das linhas 117 a 120 por:

```tex
\indent O ponto focal do paralelismo neste trabalho concentra-se exclusivamente na etapa de ordenação dos blocos. Após a divisão do vetor em blocos de tamanho 4 ou 5, cada bloco é ordenado de acordo com seu tamanho: blocos pares usam o Algoritmo 1 e blocos ímpares usam o Algoritmo 2. A etapa final aplica o Algoritmo 3 sobre os blocos já ordenados para formar o vetor final.

Para escolher a combinação de algoritmos, a implementação avalia todas as combinações válidas respeitando a restrição de não repetição entre Algoritmo 1, Algoritmo 2 e Algoritmo 3. Essa avaliação é feita em modo sequencial, usando 30 vetores aleatórios de 15000 elementos, com valores no intervalo $[0,100000]$. A combinação com menor tempo médio nessa etapa é então usada nas execuções sequencial, com \textit{std::thread} e com \textit{OpenMP}.
```

## 2. Remover o trecho antigo sobre `2000` e `5`

### Problema

O trecho marcado como `REVER` está desatualizado:

```tex
foi implementada uma etapa inicial que utiliza amostras de 2.000 elementos avaliadas ao longo de 5 iterações.
```

### Substituir as linhas 139 a 141 por:

```tex
Essa etapa de seleção foi mantida separada da comparação final entre as três versões. Assim, primeiro identifica-se a combinação mais eficiente entre as possibilidades permitidas e, em seguida, mede-se o tempo médio das versões sequencial, \textit{std::thread} e \textit{OpenMP} usando os mesmos vetores de entrada para garantir uma comparação justa.
```

## 3. Corrigir a combinação fixa afirmada no artigo

### Problema

O artigo afirma:

```tex
Definiu-se o \textit{Merge Sort} para os blocos de tamanho par ... Para os blocos de tamanho ímpar utilizou-se o \textit{Selection Sort}.
```

Isso não está alinhado com a implementação atual. O código não fixa sempre `MergeSort` para pares e `SelectionSort` para ímpares. Ele escolhe dinamicamente a melhor combinação por benchmark.

### Substituir as linhas 128 a 130 por:

```tex
Como a escolha é feita por medição, a combinação final não é definida apenas pela análise assintótica dos algoritmos. O programa testa todas as combinações permitidas, calcula o tempo médio de cada uma e seleciona a menor média observada. Dessa forma, a decisão considera o comportamento real da implementação para blocos pequenos, nos quais constantes, custo de chamada e movimentação de dados podem ser mais relevantes do que apenas a complexidade assintótica.
```

Se depois de executar `make benchmark` os resultados confirmarem uma combinação específica para todos os tamanhos, acrescente:

```tex
Nos experimentos executados, a combinação selecionada para todos os tamanhos foi: Algoritmo 1 = \textbf{preencher com o resultado}, Algoritmo 2 = \textbf{preencher com o resultado} e Algoritmo 3 = \textbf{preencher com o resultado}.
```

## 4. Corrigir a explicação da versão com `std::thread`

### Problema

O artigo afirma que a versão com threads divide os blocos pelo número de núcleos e cria um número fixo de \textit{worker threads}. O código atual não faz isso. Em `run_threads`, ele cria uma `std::thread` para cada bloco.

### Substituir a linha 151 por:

```tex
Na implementação baseada em \textit{threads} nativas, utilizando \textit{std::thread}, o paralelismo é gerenciado explicitamente. Após a criação dos blocos, o programa instancia uma \textit{thread} para cada bloco em `blocks_after_sort`. Cada \textit{thread} executa de forma independente o algoritmo correspondente ao tamanho do bloco: o Algoritmo 1 para blocos pares e o Algoritmo 2 para blocos ímpares. Ao final, o programa chama `join()` em todas as \textit{threads} antes de executar a etapa final de união dos blocos.
```

## 5. Corrigir a explicação da versão OpenMP

### Problema

O artigo diz que os blocos usam `Merge` ou `Selection`, mas isso depende da combinação escolhida dinamicamente. A implementação OpenMP usa `#pragma omp parallel for` sobre os índices dos blocos.

### Substituir a linha 153 por:

```tex
Na versão com \textit{OpenMP}, a ordenação dos blocos é paralelizada com a diretiva `#pragma omp parallel for`. O laço percorre os índices do vetor de blocos e cada iteração ordena um bloco de forma independente, escolhendo o Algoritmo 1 ou o Algoritmo 2 conforme o tamanho do bloco. Ao término do laço paralelo, a barreira implícita do \textit{OpenMP} garante que todos os blocos estejam ordenados antes da execução da etapa final de união.
```

## 6. Corrigir o trecho contraditório sobre `QuickSort` e `MergeSort`

### Problema

O artigo afirma que `MergeSort` seria pior, mas justifica dizendo que ele aproveita blocos já ordenados. Isso é contraditório. Além disso, a implementação só faz união eficiente par-a-par quando o Algoritmo 3 escolhido é `MergeSort`; para `QuickSort` ou `InsertionSort`, ela concatena os blocos e ordena o vetor inteiro.

### Substituir as linhas 143 a 145 por:

```tex
\indent Na etapa final, a implementação possui dois comportamentos. Quando o Algoritmo 3 selecionado é o \textit{Merge Sort}, os blocos já ordenados são intercalados progressivamente, aproveitando a ordenação interna de cada bloco. Quando o Algoritmo 3 selecionado é \textit{Quick Sort} ou \textit{Insertion Sort}, os blocos são primeiro concatenados e o vetor resultante é ordenado novamente. Por isso, a escolha da etapa final foi mantida experimental: o programa mede as combinações válidas e seleciona a de menor tempo médio.
```

## 7. Corrigir figuras vazias e referências pendentes

### Problema

Há figuras/tabelas com `\includegraphics{}` vazio e texto com `inserir referencia`:

- tabela da combinação ótima;
- figura de comparação de tempos;
- texto `\textbf{REVER}`;
- labels repetidos como `\label{grafico}` em mais de uma figura.

### Substituir a tabela vazia por uma referência ao CSV:

```tex
A combinação escolhida em cada tamanho é registrada no arquivo `results/benchmark_results.csv`, enquanto o ranking completo das combinações é registrado em `results/selection_ranking.csv`. Esses arquivos permitem verificar qual combinação foi selecionada e qual foi o tempo médio de cada alternativa.
```

### Substituir a figura vazia dos resultados por:

```tex
\begin{figure}[H]
    \centering
    \includegraphics[width=0.8\linewidth]{results/chart_comparison.png}
    \caption{Comparação do tempo médio entre as versões sequencial, \textit{std::thread} e \textit{OpenMP}.}
    \label{fig:comparacao_versoes}
\end{figure}
```

Use labels únicos, por exemplo:

```tex
\label{fig:quick_sort}
\label{fig:comparacao_versoes}
\label{fig:ranking_combinacoes}
```

## 8. Corrigir a seção de resultados

### Problema

`results/` está vazio. Então o artigo ainda não pode afirmar ganho substancial nem percentual de redução.

### Substituir as linhas 168 a 177 por:

```tex
Após a execução de `make benchmark`, os tempos médios das três versões são gravados em `results/benchmark_results.csv` e consolidados em `results/averages.dat`. O gráfico de comparação é gerado por `make plot` a partir desses dados.

\begin{figure}[H]
    \centering
    \includegraphics[width=0.8\linewidth]{results/chart_comparison.png}
    \caption{Comparação do tempo médio entre as versões sequencial, \textit{std::thread} e \textit{OpenMP}.}
    \label{fig:comparacao_versoes}
\end{figure}

Com os resultados gerados, esta seção deve apresentar a comparação entre os tempos médios obtidos. Caso a versão paralela seja mais rápida, informe o ganho percentual com base nos valores do CSV. Caso contrário, discuta o custo de criação de \textit{threads}, a pequena granularidade dos blocos de 4 e 5 elementos e o impacto do paralelismo em tarefas muito curtas.
```

Depois de rodar o benchmark, substitua o último parágrafo por algo concreto:

```tex
Nos resultados obtidos, a versão \textbf{preencher} apresentou o menor tempo médio na maior parte dos tamanhos avaliados. Para o tamanho de \textbf{preencher}, o tempo médio sequencial foi de \textbf{preencher}s, contra \textbf{preencher}s em \textit{std::thread} e \textbf{preencher}s em \textit{OpenMP}. Isso representa uma variação de \textbf{preencher}\% em relação à versão sequencial.
```

## 9. Completar a conclusão sem inventar resultado

### Problema

A seção de conclusão está vazia.

### Substituir a seção por:

```tex
\section{Conclusão}

\indent Este trabalho implementou um algoritmo de ordenação por blocos com três formas de execução: sequencial, paralela com \textit{std::thread} e paralela com \textit{OpenMP}. A implementação divide dinamicamente o vetor de entrada em blocos de tamanho 4 ou 5, ordena cada bloco conforme sua paridade e aplica uma etapa final de consolidação para produzir o vetor ordenado.

Também foi implementado um processo de seleção experimental da combinação de algoritmos. Em vez de escolher apenas por complexidade teórica, o programa mede todas as combinações válidas, respeitando a restrição de não repetir algoritmos entre as três etapas, e seleciona a de menor tempo médio.

Os resultados devem ser interpretados considerando que os blocos possuem tamanho muito pequeno. Assim, o custo de criação e sincronização das \textit{threads} pode reduzir ou até anular o benefício do paralelismo. Após a execução completa dos experimentos, os dados gerados em `results/benchmark_results.csv` e os gráficos produzidos por `make plot` permitem comparar objetivamente as três abordagens.
```

Quando os resultados reais estiverem disponíveis, acrescente:

```tex
Com base nos tempos medidos, conclui-se que \textbf{preencher com a versão vencedora e a justificativa baseada nos dados}. Esse resultado mostra que \textbf{preencher com a interpretação dos dados}.
```

## 10. Ajustar o texto sobre ambiente

### Problema

O artigo informa `macOS 26.4.1`, mas essa informação não foi confirmada por comando nesta análise.

### Substituição segura:

```tex
Os experimentos foram executados em ambiente macOS, com compilação em C++17 e suporte a \textit{OpenMP} por meio da biblioteca `libomp`. A configuração de hardware utilizada deve ser registrada conforme a máquina em que o benchmark final for executado.
```

Se quiser manter a configuração exata, confirme antes com:

```bash
sw_vers
sysctl -n machdep.cpu.brand_string
sysctl -n hw.memsize
```

## 11. Checklist antes de fechar o artigo

1. Rodar `make benchmark`.
2. Rodar `make plot`.
3. Confirmar que existem:
   - `results/benchmark_results.csv`
   - `results/selection_ranking.csv`
   - `results/averages.dat`
   - `results/chart_comparison.png`
   - `results/chart_selection.png`
   - `results/chart_selection_ranking.png`
   - `results/chart_selection_gain.png`
   - `results/chart_selection_choices.png`
4. Substituir todos os `\textbf{REVER}`.
5. Remover `\includegraphics{}` vazio.
6. Garantir que cada `\label{...}` seja único.
7. Não afirmar ganho percentual sem copiar o valor real do CSV.
