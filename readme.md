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
- `averages.dat`

### `benchmark_results.csv`

Contém:

- tamanho do vetor
- média da versão sequencial
- média da versão com `std::thread`
- média da versão com `OpenMP`
- melhor combinação escolhida para aquele tamanho

### `averages.dat`

Contém os dados num formato simples para o `gnuplot`.

## 4. Gerar o gráfico

Depois do benchmark:

```bash
make plot
```

Isso gera:

- `results/chart.png`

O gráfico mostra:

- `Sequencial`
- `std::thread`
- `OpenMP`

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
