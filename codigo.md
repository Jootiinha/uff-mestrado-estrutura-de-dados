# Explicação do Código

## 1. Visão geral

Este projeto implementa um algoritmo de ordenação por blocos para atender ao enunciado em [enunciado_trabalho.md](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/enunciado_trabalho.md:1).

A ideia central é:

1. gerar ou receber um vetor de inteiros;
2. dividir esse vetor aleatoriamente em blocos de tamanho `4` ou `5`;
3. ordenar cada bloco com um algoritmo diferente, dependendo se o tamanho do bloco é par ou ímpar;
4. unir os blocos ordenados usando um terceiro algoritmo;
5. comparar três modos de execução:
   - sequencial;
   - paralelo com `std::thread`;
   - paralelo com `OpenMP`.

O ponto de entrada manual fica em [run.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/run.cpp:1) e o benchmark oficial fica em [benchmark.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/benchmark.cpp:1). Quase toda a regra principal vive em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:1).

## 2. Estrutura do projeto

### Arquivos principais

- [run.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/run.cpp:1)
  Executa um caso pequeno, imprime blocos, tempos, ranking das combinações e validações.

- [benchmark.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/benchmark.cpp:1)
  Executa o experimento oficial para os tamanhos pedidos no trabalho, calcula médias e salva os resultados.

- [Makefile](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/Makefile:1)
  Centraliza compilação, benchmark, geração de gráficos e limpeza.

- [plot_results.gp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/plot_results.gp:1)
  Gera os gráficos em PNG a partir dos dados médios do benchmark.

- [readme.md](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/readme.md:1)
  Explica como compilar, executar e gerar resultados.

### Biblioteca de ordenação

- [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:1)
  Núcleo do projeto. Contém os tipos, a lógica de particionamento, a execução sequencial/paralela e a seleção da melhor combinação de algoritmos.

- [libraries/insertion_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/insertion_sort.h:1)
  Implementação do Insertion Sort.

- [libraries/selection_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/selection_sort.h:1)
  Implementação do Selection Sort.

- [libraries/bubble_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/bubble_sort.h:1)
  Implementação do Bubble Sort.

- [libraries/merge_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/merge_sort.h:1)
  Implementação do Merge Sort.

- [libraries/quick_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/quick_sort.h:1)
  Implementação do Quick Sort.

- [libraries/utils.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/utils.h:1)
  Gera vetores aleatórios para a execução manual.

- [libraries/timer.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/timer.h:1)
  Mede tempo de parede e tempo de CPU.

## 3. Modelo mental do algoritmo

O projeto modela o problema com três escolhas de algoritmos:

- `Algoritmo1`: usado em blocos pares.
- `Algoritmo2`: usado em blocos ímpares.
- `Algoritmo3`: usado na etapa final para juntar os blocos.

Essas escolhas são representadas pela struct `BlockSortCombination` em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:25).

Ela guarda:

- `even_block`
- `odd_block`
- `final_stage`

Exemplo de combinação:

- blocos pares: `InsertionSort`
- blocos ímpares: `BubbleSort`
- etapa final: `MergeSort`

O código também impõe a regra do enunciado de não reutilizar o mesmo algoritmo em mais de um papel.

## 4. Tipos principais em `block_sort.hpp`

### `BlockSortAlgorithm`

Enum com os algoritmos possíveis:

- `InsertionSort`
- `SelectionSort`
- `BubbleSort`
- `MergeSort`
- `QuickSort`

Ele existe para o código poder escolher algoritmos dinamicamente em vez de fixar uma implementação em tempo de compilação.

### `BlockSortMode`

Enum com os modos de execução:

- `Sequential`
- `Threads`
- `OpenMP`

Serve para a função `run_mode` despachar a execução correta.

### `BlockSortBenchmarkSummary`

Guarda:

- a combinação usada;
- o tempo médio daquela combinação.

É o tipo retornado quando o projeto avalia qual combinação é mais eficiente.

### `BlockSortExecution`

Guarda tudo que importa numa execução:

- `blocks_before_sort`
- `blocks_after_sort`
- `result`

Isso permite inspecionar não só o vetor final, mas também como ele foi quebrado em blocos e como esses blocos ficaram depois da ordenação.

## 5. Como o vetor é dividido em blocos

### `can_partition_into_blocks_4_or_5`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:126) que responde:

"um vetor de tamanho `n` pode ser decomposto em blocos de 4 e 5?"

Ela usa programação dinâmica:

- `dp[i] = true` significa que o tamanho `i` é alcançável;
- parte de `dp[0] = true`;
- avança somando `4` e `5`.

Isso evita construir partições impossíveis.

Exemplo:

- `8` pode: `4 + 4`
- `9` pode: `4 + 5`
- `10` pode: `5 + 5`
- `11` nao pode, porque não existe combinação formada apenas por `4` e `5`

### `partition_blocks`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:147).

Fluxo:

1. valida se o tamanho total pode ser particionado;
2. percorre o vetor do início ao fim;
3. em cada passo, avalia quais tamanhos (`4` ou `5`) ainda permitem completar o restante do vetor;
4. escolhe aleatoriamente entre as opções válidas usando `std::mt19937`;
5. cria um novo bloco com aquele pedaço do vetor.

Ponto importante:

- a escolha é aleatória;
- mas nunca quebra a viabilidade do restante.

Ou seja, o algoritmo é aleatório sem ficar inconsistente.

## 6. Como o projeto escolhe o algoritmo de cada bloco

### `apply_algorithm`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:180).

Ela recebe:

- um `vector<int>&`
- um `BlockSortAlgorithm`

Depois faz um `switch` e chama a implementação correspondente:

- `InsertionSort().sort(data)`
- `SelectionSort().sort(data)`
- `BubbleSort().sort(data)`
- `MergeSort().sort(data)`
- `QuickSort().sort(data)`

Isso funciona como um despachante central.

### Regra par/ímpar

Nos três modos de execução, a regra é a mesma:

- se o bloco tem tamanho par, usa `combination.even_block`;
- se o bloco tem tamanho ímpar, usa `combination.odd_block`.

Como os blocos sempre têm tamanho `4` ou `5`, isso na prática significa:

- bloco `4` usa algoritmo de par;
- bloco `5` usa algoritmo de ímpar.

## 7. Como a etapa final junta os blocos

### `merge_two_sorted_vectors`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:196).

Ela faz o merge clássico de dois vetores já ordenados:

- compara ponteiros `i` e `j`;
- insere o menor valor;
- consome o restante ao final.

Essa função é importante quando `Algoritmo3` é `MergeSort`, porque a etapa final não precisa reordenar tudo do zero.

### `merge_blocks`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:222).

Ela tem dois comportamentos:

#### Quando `final_stage_algorithm == MergeSort`

O código aproveita que cada bloco já está ordenado e faz uma fusão em níveis:

1. junta blocos de dois em dois;
2. cria um próximo nível de blocos maiores;
3. repete até sobrar um único vetor.

Isso é mais coerente com a ideia de merge e evita ordenar novamente todo o conjunto sem necessidade.

#### Quando `final_stage_algorithm` é `InsertionSort` ou `QuickSort`

O código:

1. achata todos os blocos em um único vetor;
2. aplica o algoritmo final sobre o vetor inteiro.

Aqui a estratégia é diferente:

- não há merge incremental;
- há uma reordenação completa do vetor achatado.

## 8. Execução sequencial

### `run_sequential`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:251).

Fluxo completo:

1. chama `partition_blocks`;
2. copia os blocos para `blocks_after_sort`;
3. percorre cada bloco em loop normal;
4. escolhe o algoritmo do bloco com base na paridade do tamanho;
5. ordena o bloco;
6. chama `merge_blocks`;
7. devolve um `BlockSortExecution`.

Essa função é a versão base do algoritmo. As paralelas preservam a mesma regra, mudando apenas a forma de ordenar os blocos.

## 9. Execução com `std::thread`

### `run_threads`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:269).

Diferença principal:

- cada bloco é ordenado em uma thread separada.

Fluxo:

1. cria a partição;
2. copia os blocos;
3. cria um vetor de `std::thread`;
4. para cada bloco, cria uma thread que aplica o algoritmo correspondente;
5. faz `join()` em todas as threads;
6. depois junta os blocos com `merge_blocks`.

Observação importante:

- o paralelismo está exatamente no ponto exigido pelo enunciado: a ordenação dos blocos;
- a montagem dos blocos e a fusão final continuam sequenciais.

## 10. Execução com `OpenMP`

### `run_openmp`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:294).

A lógica é a mesma da versão com threads, mas usando:

```cpp
#pragma omp parallel for
```

Isso paraleliza o loop que percorre `execution.blocks_after_sort`.

Cada iteração:

- pega um bloco;
- decide o algoritmo;
- ordena localmente aquele bloco.

Vantagem dessa abordagem:

- menos código manual de gerenciamento de threads.

Trade-off:

- depende da configuração correta do compilador e das flags de OpenMP.

## 11. Como a melhor combinação é escolhida

Essa é uma das partes mais importantes do projeto, porque o enunciado não pede apenas implementar o algoritmo, mas também descobrir a combinação mais eficiente respeitando as restrições.

### `is_valid_combination`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:79).

Ela valida:

- algoritmos permitidos para blocos pares;
- algoritmos permitidos para blocos ímpares;
- algoritmos permitidos para etapa final;
- proibição de repetir algoritmo entre os três papéis.

### `valid_combinations`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:102).

Ela enumera todas as combinações possíveis e filtra apenas as válidas.

### `benchmark_combination`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:339).

Ela mede o tempo médio de uma combinação:

1. executa `run_sequential` sobre vários vetores de entrada;
2. usa sementes controladas para particionamento;
3. valida o resultado de cada execução;
4. soma os tempos e divide pela quantidade de amostras.

Ponto relevante:

- a seleção da melhor combinação é feita em modo sequencial;
- depois a combinação vencedora é usada para comparar os três modos de execução.

Isso faz sentido porque a combinação escolhida representa a política do algoritmo, não o mecanismo de paralelismo.

### `rank_combinations`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:366).

Ela ordena as combinações da mais rápida para a mais lenta.

### `select_best_combination`

Função em [libraries/block_sort.hpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/block_sort.hpp:383).

Ela devolve a primeira colocada do ranking.

## 12. Validações de correção

O projeto não mede só tempo. Ele também checa se o algoritmo continua correto.

### `are_valid_block_sizes`

Verifica se todos os blocos têm tamanho `4` ou `5`.

### `is_sorted_non_decreasing`

Verifica se o vetor final está em ordem não decrescente usando `std::is_sorted`.

### `preserves_elements`

Verifica se o vetor final preserva exatamente os mesmos elementos do original:

1. copia `original` e `result`;
2. ordena as duas cópias com `std::sort`;
3. compara os vetores.

Isso detecta erros como:

- perda de elementos;
- duplicação acidental;
- troca de conteúdo.

## 13. O que `run.cpp` faz

O arquivo [run.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/run.cpp:1) é uma execução demonstrativa.

### Entrada

Aceita:

- tamanho do vetor;
- seed opcional.

Se o tamanho não puder ser particionado em blocos 4/5, o programa aborta com erro.

### Geração dos dados

Usa `Utils::preencherVetor` para gerar um vetor aleatório pequeno.

### Escolha automática da melhor combinação

Antes de rodar as três versões, ele:

1. gera entradas auxiliares de benchmark com `make_benchmark_inputs`;
2. calcula o ranking com `rank_combinations`;
3. pega a melhor combinação.

Ou seja, a execução manual não testa uma combinação arbitrária; ela já usa a combinação considerada mais eficiente pela lógica do projeto.

### Execução das três variantes

Depois ele roda:

- `run_sequential`
- `run_threads`
- `run_openmp`

medindo os tempos com `Timer`.

### Comparações

O programa compara:

- se o particionamento foi o mesmo entre as versões;
- se o resultado final foi o mesmo.

Isso é importante porque o paralelismo não deve alterar a semântica do algoritmo.

### Impressão detalhada

As funções auxiliares:

- `print_vector`
- `print_blocks`
- `print_ranking`
- `print_execution_summary`

servem para tornar a execução inspecionável no terminal.

Elas mostram:

- blocos antes da ordenação;
- blocos depois da ordenação;
- lista final;
- validações de correção.

## 14. O que `benchmark.cpp` faz

O arquivo [benchmark.cpp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/benchmark.cpp:1) implementa o experimento pedido na segunda parte do trabalho.

### Parâmetros do benchmark

Ele fixa:

- tamanhos `15000`, `16000`, `17000`, `18000`, `19000`, `20000`;
- `30` iterações por tamanho;
- faixa de valores `[0, 100000]`.
- cinco vetores auxiliares de tamanho `2000` para seleção;
- cinco vetores auxiliares diferentes, também de tamanho `2000`, para validação.

Esses números batem com o enunciado.

### Estratégia do benchmark

Para cada tamanho:

1. gera os 30 vetores-base;
2. gera um conjunto menor para selecionar a melhor combinação;
3. mede o tempo total e guarda o ranking completo da seleção;
4. valida a combinação vencedora contra uma combinação fixa usando outros vetores;
5. roda os três modos com os mesmos dados do benchmark principal;
6. calcula as médias;
7. salva resultados parciais.

A combinação fixa usada como referência é:

- blocos pares: `InsertionSort`;
- blocos ímpares: `SelectionSort`;
- etapa final: `MergeSort`.

Separar as amostras de seleção e validação evita justificar a escolha apenas
com os mesmos dados usados para definir a vencedora.

Nos gráficos, o tamanho do eixo horizontal é o da execução principal. As
amostras auxiliares permanecem pequenas para limitar o custo do ranking.

### `average_runtime`

Essa função mede a média de um modo específico.

Ela:

- recebe o modo;
- recebe todas as entradas;
- executa `BlockSort::run_mode`;
- valida cada saída;
- calcula a média do tempo total.

### Persistência dos resultados

`write_results_csv` grava:

- tamanho;
- custo total da seleção;
- tempo da vencedora nas amostras de seleção e validação;
- tempo da combinação fixa na validação;
- ganho percentual da seleção dinâmica;
- média sequencial;
- média com threads;
- média com OpenMP;
- combinação vencedora.

`write_selection_ranking_csv` grava todas as combinações avaliadas, suas
posições e seus tempos médios.

`write_results_dat` grava um formato simples para o `gnuplot`.

Essa gravação é incremental. Se o benchmark for interrompido no meio, o que já foi processado continua salvo em `results/`.

## 15. Papel dos algoritmos individuais

Cada arquivo em `libraries/*_sort.h` encapsula um algoritmo clássico.

### `InsertionSort`

Arquivo: [libraries/insertion_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/insertion_sort.h:1)

Boa escolha para conjuntos pequenos porque:

- tem implementação simples;
- costuma se comportar bem em entradas pequenas;
- o custo de overhead é baixo.

### `SelectionSort`

Arquivo: [libraries/selection_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/selection_sort.h:1)

Faz busca do menor elemento restante e troca para a posição correta.

### `BubbleSort`

Arquivo: [libraries/bubble_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/bubble_sort.h:1)

É simples e tem a otimização de encerrar cedo quando não há mais trocas.

### `MergeSort`

Arquivo: [libraries/merge_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/merge_sort.h:1)

É recursivo, divide o vetor e depois faz merge.

### `QuickSort`

Arquivo: [libraries/quick_sort.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/quick_sort.h:1)

Usa o último elemento como pivô e particiona o vetor recursivamente.

## 16. Papel de `Utils` e `Timer`

### `Utils`

Arquivo: [libraries/utils.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/utils.h:1)

Responsabilidade:

- preencher vetores com números aleatórios.

Observação:

- ele usa `rand()` e `srand()`;
- já `block_sort.hpp` usa `mt19937` para benchmark e particionamento.

Na prática, o benchmark principal está usando a geração moderna e determinística com seed explícita.

### `Timer`

Arquivo: [libraries/timer.h](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/libraries/timer.h:1)

Responsabilidade:

- medir tempo decorrido real;
- medir tempo de CPU;
- retornar ambos em um `vector<double>`.

Em `run.cpp`, o código imprime `elapsed[0]`, que é o tempo de parede baseado em `std::chrono`.

## 17. Compilação e execução

O [Makefile](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/Makefile:1) define:

- `make run`
  Compila `run.cpp`.

- `make benchmark`
  Compila `benchmark.cpp`, cria `results/` e executa o benchmark.

- `make plot`
  Roda o `gnuplot` para gerar os gráficos em `results/`.

- `make clean`
  Remove binários e resultados.

No macOS, o Makefile ajusta as flags de `OpenMP` usando o caminho do `libomp` instalado com Homebrew.

## 18. Como os gráficos são gerados

O arquivo [plot_results.gp](/Users/joaocrm/Documents/dev/mestrado/uff-mestrado-estrutura-de-dados/plot_results.gp:1) lê `results/averages.dat` e gera cinco gráficos:

- custo total do ranking dinâmico;
- comparação entre `Sequencial`, `std::thread` e `OpenMP`;
- cinco primeiras posições do ranking de combinações;
- ganho da combinação selecionada contra a referência fixa;
- algoritmos escolhidos nos blocos pares, blocos ímpares e etapa final.

Os gráficos usam o tamanho do vetor da execução principal no eixo horizontal.
As métricas verticais variam entre:

- tempo total ou médio em segundos;
- redução percentual de tempo;
- algoritmo escolhido em cada etapa.

## 19. Fluxo completo do projeto

Se alguém quiser entender o código ponta a ponta, o fluxo real é:

1. `make run` ou `make benchmark`;
2. compilação do arquivo principal;
3. inclusão de `block_sort.hpp`;
4. geração dos dados;
5. escolha da melhor combinação válida;
6. particionamento aleatório do vetor em blocos 4/5;
7. ordenação de cada bloco com base na paridade;
8. junção final dos blocos;
9. validação da saída;
10. impressão no terminal ou persistência em arquivo.

## 20. Decisões de projeto mais importantes

### 1. Separar algoritmo principal e programas clientes

`block_sort.hpp` concentra a regra de negócio.

`run.cpp` e `benchmark.cpp` ficam como clientes dessa biblioteca.

Isso evita duplicar lógica.

### 2. Escolher a melhor combinação dinamicamente

O projeto não fixa previamente a combinação de algoritmos. Ele mede e escolhe.

Isso deixa a solução mais alinhada ao enunciado, que pede a combinação mais eficiente.

### 3. Manter o mesmo comportamento nas três versões

Sequencial, threads e OpenMP compartilham:

- mesma lógica de partição;
- mesma combinação de algoritmos;
- mesma etapa final.

O que muda é apenas o modo de paralelizar a ordenação dos blocos.

### 4. Validar correção antes de confiar em performance

O código sempre tenta garantir:

- blocos corretos;
- ordenação correta;
- preservação dos elementos.

Isso impede comparar tempos de uma implementação que esteja errada.

## 21. Limitações e observações do código atual

### Mistura de estilos de geração aleatória

`Utils` usa `rand()`, enquanto o núcleo usa `mt19937`.

Isso não quebra o projeto, mas mostra que a geração aleatória da execução manual e a do benchmark seguem caminhos diferentes.

### `run.cpp` imprime "OpenMP: habilitado em compilacao."

Essa mensagem é fixa no código. Ela não verifica em tempo de execução se o binário realmente foi compilado com suporte ativo a OpenMP; apenas assume isso com base na compilação esperada.

### O paralelismo está só na ordenação dos blocos

Isso é proposital e coerente com o enunciado. A partição e a etapa final continuam sequenciais.

### `merge_blocks` tem duas estratégias diferentes

Quando a etapa final é `MergeSort`, o código usa fusão incremental de blocos.

Quando a etapa final é `InsertionSort` ou `QuickSort`, ele concatena tudo e ordena de novo.

Isso é uma decisão prática do código atual e impacta diretamente o desempenho de cada combinação.

## 22. Resumo final

Este projeto é uma implementação de ordenação por blocos com três variações de execução:

- sequencial;
- paralela com `std::thread`;
- paralela com `OpenMP`.

O coração do sistema está em `block_sort.hpp`, que:

- gera partições válidas em blocos 4/5;
- testa combinações permitidas de algoritmos;
- escolhe a melhor combinação;
- executa o algoritmo nos três modos;
- valida se o resultado está correto.

`run.cpp` serve para inspecionar a execução detalhadamente.

`benchmark.cpp` serve para produzir as médias pedidas no trabalho e alimentar os gráficos finais.
