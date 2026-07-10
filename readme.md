# Como executar o trabalho

## Pré-requisitos para MacOs

O projeto usa:

- `g++` com suporte a C++17
- `make`
- `gnuplot`
- `libomp` para validar a versão com `OpenMP`

Para instalar:

```bash
brew install libomp
brew install gnuplot
```


## 1. Execução

Para compilar a execução manual:

```bash
make run
```

Para rodar um caso pequeno:

```bash
./run 20 7
```

Significado:

- `20` = tamanho do vetor
- `7` = seed usada no particionamento em blocos

Esse comando:

- gera um vetor aleatório;
- escolhe dinamicamente a melhor combinação de algoritmos;
- divide o vetor em blocos `4/5`;
- executa as versões sequencial, `std::thread` e `OpenMP`;
- compara os resultados entre elas;
- imprime validações no terminal.

Para confirmar que `OpenMP` está ativo de verdade, a saída deve conter:

```text
OpenMP: habilitado em compilacao.
```

## 2. Rodar o benchmark

Para executar o benchmark completo:

```bash
make benchmark
```

Esse benchmark faz exatamente o que o enunciado pede:

- tamanhos `15000`, `16000`, `17000`, `18000`, `19000`, `20000`
- `30` vetores aleatórios por tamanho
- valores no intervalo `[0, 100000]`
- comparação entre:
  - versão sequencial
  - versão com `std::thread`
  - versão com `OpenMP`

Observação:

- o benchmark pode demorar bastante e não existe barra de progresso
- os resultados são gravados incrementalmente em `results/`, então mesmo que você interrompa depois de algum tamanho, o que já foi concluído permanece salvo.

## 3. Arquivos gerados pelo benchmark

Depois do `make benchmark`, os principais arquivos ficam em `results/`:

- `benchmark_results.csv`
- `selection_ranking.csv`
- `averages.dat`

### `benchmark_results.csv`

Contém:

- tamanho do vetor
- tempo total gasto para avaliar todas as combinações
- média da combinação vencedora na amostra de seleção
- comparação da vencedora com uma combinação fixa em uma amostra independente
- ganho percentual da seleção dinâmica
- média da versão sequencial
- média da versão com `std::thread`
- média da versão com `OpenMP`
- melhor combinação escolhida para aquele tamanho

### `selection_ranking.csv`

Contém o ranking completo das combinações para cada caso do benchmark, com:

- posição no ranking
- tempo médio
- algoritmos usados nos blocos pares, blocos ímpares e etapa final

A amostra de validação usa vetores diferentes dos usados para selecionar a
combinação. A referência fixa é `pares=InsertionSort`,
`impares=SelectionSort`, `final=MergeSort`.

Tanto a seleção quanto a validação usam cinco vetores auxiliares de tamanho
`2000`. O tamanho exibido no eixo horizontal identifica a execução principal
à qual aquela escolha foi aplicada.

### `averages.dat`

Contém os dados num formato simples para o `gnuplot`.

## 4. Gerar os gráficos

Depois do benchmark:

```bash
make plot
```

Isso gera:

- `results/chart_selection.png`
- `results/chart_comparison.png`
- `results/chart_selection_ranking.png`
- `results/chart_selection_gain.png`
- `results/chart_selection_choices.png`

Os gráficos mostram:

- `chart_selection.png`: o custo total de avaliar e ordenar todas as combinações
- `chart_comparison.png`: a comparação entre as versões sequencial, `std::thread` e `OpenMP`
- `chart_selection_ranking.png`: os tempos das cinco primeiras posições do ranking
- `chart_selection_gain.png`: o ganho percentual da combinação selecionada contra a referência fixa em dados independentes
- `chart_selection_choices.png`: os algoritmos escolhidos para blocos pares, blocos ímpares e etapa final

No gráfico de ganho, um valor positivo indica que a seleção dinâmica foi mais
rápida que a combinação fixa. Um valor negativo indica que a referência fixa
foi mais rápida naquele caso.

## 5. Fluxo recomendado para atender ao trabalho

Ordem prática:

```bash
make run
./run 20 7
make benchmark
make plot
```

## 6. Limpeza

Para remover binários e resultados:

```bash
make clean
```

Isso apaga:

- `run`
- `benchmark`
- `results/`
