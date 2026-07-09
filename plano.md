# Plano de implementação

## Estado atual validado

O repositório já possui partes prontas que devem ser avaliadas antes de escrever código novo.

### Reutilizar

- `libraries/bubble_sort.h`
- `libraries/insertion_sort.h`
- `libraries/merge_sort.h`
- `libraries/quick_sort.h`
- `libraries/selection_sort.h`
- `libraries/timer.h`
- `run.cpp` como ponto simples de execução manual

Esses arquivos já entregam:

- implementações dos algoritmos básicos pedidos no enunciado;
- medição de tempo com `std::chrono` em `Timer`;
- geração simples de dados de teste;
- um executável de teste rápido em `run.cpp`.

### Adaptar

- `benchmark.cpp`
- `libraries/utils.h`
- `Makefile`
- `plot_results.gp`

Esses pontos já têm utilidade, mas ainda refletem o trabalho antigo de benchmark de algoritmos isolados, não o algoritmo por blocos exigido agora.

### Remover ou substituir

- lógica em `run.cpp` que escolhe um algoritmo isolado por nome;
- lógica em `benchmark.cpp` que compara diretamente `bubble_sort`, `insertion_sort`, `merge_sort`, `quick_sort` e `selection_sort` como algoritmos independentes;
- geração de tamanhos de `5000` até `20000` em `benchmark.cpp`;
- qualquer saída em `results/` que represente apenas o benchmark antigo, se ela atrapalhar a leitura dos novos resultados;
- qualquer regra de `plot_results.gp` baseada no benchmark antigo, se ela não servir para sequencial vs `threads` vs `OpenMP`.

Não faz sentido manter código de comparação dos algoritmos isolados se o trabalho agora exige:

- seleção dinâmica de combinação;
- ordenação por blocos 4/5;
- comparação entre versões sequencial, `std::thread` e `OpenMP`.

## Objetivo

Implementar um programa em C++ que:

1. recebe um conjunto de números;
2. divide os números em blocos aleatórios de tamanho 4 ou 5;
3. ordena cada bloco conforme sua paridade;
4. une os blocos ordenados para reconstruir o conjunto final ordenado;
5. entrega duas versões paralelas da etapa de ordenação dos blocos:
   - uma com `std::thread`;
   - uma com `OpenMP`.
6. entrega também uma versão sequencial para comparação;
7. executa benchmark nos tamanhos exigidos e salva os resultados médios para geração do gráfico.

## Escolha dos algoritmos

A combinação não será fixa. O programa deverá escolher dinamicamente a melhor combinação válida com base em medição real de tempo.

### Justificativa

- O enunciado pede a combinação mais eficiente.
- Em vez de assumir essa combinação por análise teórica, o plano passa a medir as combinações na prática.
- Isso torna a solução mais defensável, porque a escolha final será baseada em tempo real de execução no ambiente usado no trabalho.
- Como o conjunto de algoritmos candidatos é pequeno, dá para testar todas as combinações válidas sem inflar demais a complexidade.

### Evidência no código atual

Já existem implementações reutilizáveis dos cinco algoritmos candidatos em `libraries/`.
Portanto, o plano deve evitar reimplementar ordenações do zero e focar no algoritmo por blocos, no despachante de combinação e no benchmark oficial.

### Combinações candidatas

O programa deve gerar todas as combinações válidas respeitando:

- `Algoritmo1` para blocos pares: `Selection Sort`, `Merge Sort` ou `Insertion Sort`
- `Algoritmo2` para blocos ímpares: `Selection Sort`, `Merge Sort` ou `Bubble Sort`
- `Algoritmo3` para união final: `Insertion Sort`, `Merge Sort` ou `Quick Sort`
- um mesmo algoritmo não pode ser reutilizado dentro da mesma combinação

Exemplo:

- se `Algoritmo1 = Merge Sort`, então `Algoritmo2` e `Algoritmo3` não podem usar `Merge Sort`.

## Estratégia de seleção dinâmica

### 1. Benchmark inicial das combinações

Antes da execução principal, o programa deve:

- montar a lista de combinações válidas;
- gerar entradas de teste aleatórias controladas;
- executar o algoritmo completo para cada combinação;
- medir o tempo total de execução;
- escolher a combinação com menor tempo.

### 2. Critério de medição

Usar `std::chrono` para medir:

- da divisão em blocos até o vetor final ordenado; ou
- no mínimo da ordenação dos blocos até a recombinação final.

Como o enunciado está focado no algoritmo inteiro, a opção mais limpa é medir o fluxo completo da combinação.

### 3. Estabilização da medição

Para reduzir ruído, o plano deve prever:

- repetir cada combinação algumas vezes;
- calcular média ou mediana;
- escolher a melhor combinação pelo menor valor agregado.

Uma solução simples e suficiente:

- executar cada combinação de 5 a 10 vezes;
- usar a média dos tempos.

### 4. Reuso da combinação vencedora

Depois da etapa de benchmark:

- armazenar a combinação vencedora;
- usar essa mesma combinação nas execuções sequencial, `std::thread` e `OpenMP`;
- exibir no terminal qual combinação foi escolhida.

## Estratégia de implementação

### 1. Definir a estrutura do programa

Criar uma implementação principal com estas responsabilidades:

- gerar ou receber o vetor de entrada;
- gerar as combinações válidas de algoritmos;
- executar o benchmark de seleção automática;
- executar a versão sequencial;
- executar a versão paralela com `std::thread`;
- executar a versão paralela com `OpenMP`;
- particionar o vetor em blocos aleatórios de 4 ou 5;
- armazenar os blocos em `std::vector<std::vector<int>>`;
- ordenar os blocos conforme a versão selecionada;
- unir os blocos ordenados em um vetor final;
- validar se o resultado está realmente ordenado;
- registrar tempos médios por tamanho de entrada.

Decisão prática:

- reaproveitar `run.cpp` como modo de execução manual e depuração;
- reaproveitar `benchmark.cpp` como modo de benchmark oficial;
- não criar novos binários se os dois arquivos atuais forem suficientes.

### 2. Implementar os algoritmos básicos

Implementar funções separadas para:

- `insertion_sort(std::vector<int>& bloco)`
- `selection_sort(std::vector<int>& bloco)`
- `bubble_sort(std::vector<int>& bloco)`
- `merge_sort(std::vector<int>& bloco)` quando ele for candidato para bloco
- `quick_sort(std::vector<int>& bloco)` quando ele for candidato para união final
- `merge_sorted_vectors(...)` ou função equivalente para intercalar blocos já ordenados

Validação do que já existe:

- `bubble_sort`, `insertion_sort`, `merge_sort`, `quick_sort` e `selection_sort` já existem;
- o que falta não são os sorts isolados, e sim a orquestração do algoritmo do enunciado.

Portanto:

- reutilizar os headers atuais como base;
- só ajustar algo neles se aparecer necessidade real de interface comum ou correção;
- não duplicar essas implementações em novos arquivos.

### 3. Implementar a divisão em blocos

Criar uma função para particionar dinamicamente o vetor:

- enquanto houver elementos restantes, escolher aleatoriamente entre bloco 4 ou 5;
- se no final não houver elementos suficientes para manter essa regra, ajustar os últimos blocos para não sobrar bloco inválido;
- registrar os blocos em um container simples.

Ponto importante:

- a lógica final da partição precisa garantir que todos os blocos tenham tamanho 4 ou 5;
- se a quantidade total de elementos não permitir isso exatamente, definir uma política clara:
  - ou restringir a entrada para tamanhos compatíveis;
  - ou tratar o último bloco com redistribuição entre os anteriores.

### 4. Representar a combinação escolhida

Criar uma estrutura simples para representar uma combinação, por exemplo:

- algoritmo para bloco par;
- algoritmo para bloco ímpar;
- algoritmo para união final;
- tempo médio medido.

Pode ser um `struct` pequeno com `enum` para os algoritmos. Não há necessidade de hierarquia de classes.

### 5. Estrutura dos modos de execução

Separar claramente três modos de execução do mesmo algoritmo:

- sequencial;
- paralelo com `std::thread`;
- paralelo com `OpenMP`.

Os três modos devem:

- usar a mesma lógica de partição;
- usar a mesma combinação vencedora de algoritmos;
- receber exatamente o mesmo vetor de entrada em cada rodada de benchmark.

Decisão de reaproveitamento:

- a estrutura geral já existente em `benchmark.cpp` para gerar vetores-base e reutilizá-los entre execuções deve ser mantida;
- o que muda é o alvo da medição: sai benchmark de sort isolado e entra benchmark do algoritmo completo por blocos.

## Paralelismo

O paralelismo deve existir apenas na ordenação dos blocos, como pede o enunciado.

### 6. Versão com `std::thread`

Plano:

- criar uma thread por bloco, ou limitar a quantidade de threads ao número de núcleos disponíveis;
- cada thread recebe referência para um bloco e aplica o algoritmo definido pela combinação vencedora:
  - tamanho par: `Algoritmo1`
  - tamanho ímpar: `Algoritmo2`
- ao final, usar `join()` em todas as threads;
- depois disso, executar a união dos blocos no fluxo principal.

Observação prática:

- como os blocos são minúsculos, criar uma thread por bloco pode gerar overhead alto;
- vale deixar no relatório que essa versão atende ao enunciado, mas pode não ser a mais eficiente em tempo absoluto se houver blocos demais.

### 7. Versão com `OpenMP`

Plano:

- usar `#pragma omp parallel for` sobre o vetor de blocos;
- cada iteração ordena um bloco independentemente;
- cada bloco usa o algoritmo definido pela combinação vencedora;
- após a região paralela, executar `Algoritmo3` para a união final.

Vantagem:

- a implementação fica menor que a versão com `std::thread`;
- o agendamento de trabalho fica mais simples.

## União dos blocos

### 8. Recombinar os blocos ordenados

Após ordenar todos os blocos:

- aplicar o algoritmo definido como `Algoritmo3`;
- se `Algoritmo3 = Merge Sort`, unir os blocos dois a dois por intercalação;
- se `Algoritmo3 = Quick Sort` ou `Insertion Sort`, achatar os blocos em um vetor e aplicar a ordenação final escolhida.

Esse passo pode ser implementado como:

- uma estratégia específica por algoritmo final; ou
- um despachante simples que chama a implementação correta.

A solução mais simples é um despachante por `enum`, sem polimorfismo.

## Benchmark obrigatório

### 9. Cenário de teste do enunciado

Implementar o benchmark exatamente com:

- tamanhos de vetor: `15000`, `16000`, `17000`, `18000`, `19000`, `20000`;
- 30 vetores diferentes para cada tamanho;
- valores aleatórios no intervalo `[0, 100000]`;
- comparação justa entre sequencial, `std::thread` e `OpenMP` usando os mesmos vetores de entrada.

### 10. Estratégia de geração de dados

Para cada tamanho:

- gerar previamente os 30 vetores aleatórios;
- armazená-los temporariamente em memória durante o benchmark daquele tamanho;
- reutilizar exatamente esses 30 vetores nas três versões.

Isso evita viés entre execuções.

### 11. Métrica coletada

Para cada tamanho e para cada versão:

- medir as 30 execuções;
- calcular a média;
- salvar uma linha de resultado com:
  - tamanho do vetor;
  - média da versão sequencial;
  - média da versão `std::thread`;
  - média da versão `OpenMP`.

### 12. Saída para gráfico

O código deve gerar um arquivo simples em `results/`, por exemplo CSV, contendo as médias.

Exemplo de colunas:

- `size`
- `sequential_ms`
- `threads_ms`
- `openmp_ms`

Isso é suficiente para o gráfico depois, sem misturar a implementação com o artigo.

## Organização sugerida dos arquivos

Como o repositório já tem `run.cpp` e `benchmark.cpp`, a menor mudança segura é:

- adaptar `run.cpp` para executar um caso simples do algoritmo por blocos e imprimir/verificar resultado;
- adaptar `benchmark.cpp` para:
  - escolher dinamicamente a melhor combinação;
  - executar a versão sequencial;
  - executar a versão com `std::thread`;
  - executar a versão com `OpenMP`;
  - salvar os resultados médios em `results/`.

Se a lógica começar a ficar grande, extrair somente o núcleo compartilhado para:

- `libraries/block_sort.hpp`

Evitar criar `block_sort.cpp` se um header simples bastar. O projeto atual já usa implementações inline em header, então o plano deve seguir o padrão existente antes de inventar nova organização.

### O que não reaproveitar

Não reaproveitar como está:

- o contrato atual de `run.cpp`, que hoje depende do nome de um sort isolado na linha de comando;
- o contrato atual de `benchmark.cpp`, que hoje mede algoritmos individuais em vez das três versões do algoritmo pedido;
- os tamanhos atuais do benchmark, que começam em `5000` e não seguem o enunciado.

## Etapas de execução

### Fase 1

- [x] validar e reaproveitar os algoritmos candidatos já existentes;
- [x] implementar versão sequencial correta;
- [x] validar partição em blocos 4/5;
- [x] validar ordenação de cada bloco;
- [x] validar merge final.

### Fase 2

- [x] implementar a geração das combinações válidas;
- [x] implementar o benchmark inicial com `std::chrono`;
- [x] selecionar automaticamente a combinação vencedora;
- [x] comparar as combinações e registrar a escolhida.

### Fase 3

- [x] extrair a ordenação dos blocos para função reutilizável;
- [x] criar versão paralela com `std::thread`;
- [x] comparar resultado com a versão sequencial.

### Fase 4

- [x] criar versão paralela com `OpenMP`;
- [x] comparar resultado com a versão sequencial.

### Fase 5

- [x] implementar o benchmark obrigatório dos tamanhos `15000` a `20000`;
- [x] gerar 30 vetores aleatórios por tamanho;
- [x] medir as três versões com os mesmos vetores;
- [x] salvar as médias em `results/`.

### Fase 6

- [x] remover ou sobrescrever o benchmark antigo que compara sorts isolados;
- [x] ajustar `plot_results.gp` apenas se ele continuar útil para os novos dados;
- [x] limpar artefatos antigos de `results/` se estiverem confundindo os resultados novos.

## Validação mínima

Antes de considerar concluído:

- testar com vetores pequenos conhecidos;
- testar com entrada aleatória;
- confirmar que o vetor final está ordenado;
- confirmar que nenhuma versão perde ou duplica elementos;
- comparar saída sequencial, `threads` e `OpenMP`;
- confirmar que a combinação escolhida automaticamente é válida;
- confirmar que não há repetição de algoritmo dentro da combinação escolhida;
- confirmar que o benchmark usa a mesma entrada base para comparar as combinações de forma justa;
- confirmar que os tamanhos usados são exatamente `15000` a `20000` de `1000` em `1000`;
- confirmar que cada tamanho roda exatamente 30 vetores diferentes;
- confirmar que o arquivo de saída contém as médias das três versões.

## Riscos e decisões importantes

### 1. Tamanho final dos blocos

Esse é o ponto mais delicado do enunciado. A implementação precisa decidir como evitar sobra inválida de 1, 2 ou 3 elementos no final.

### 2. Overhead de paralelismo

Como os blocos têm apenas 4 ou 5 elementos, o custo de paralelizar pode ser maior que o custo de ordenar. Isso não invalida a solução, mas deve ser explicitado no relatório.

### 3. Ruído na medição

Medições muito curtas podem oscilar bastante. Por isso, o benchmark inicial precisa repetir execuções e usar média ou mediana.

### 4. Justiça da comparação

Todas as combinações devem ser comparadas com a mesma entrada ou com o mesmo conjunto de entradas, senão a escolha pode ficar enviesada pela aleatoriedade dos dados.

### 5. Custo total do benchmark

O benchmark completo terá:

- 6 tamanhos de entrada;
- 30 vetores por tamanho;
- 3 versões do algoritmo;
- mais a fase extra de seleção dinâmica da melhor combinação.

Isso pode deixar a execução demorada, então convém separar:

- um modo curto para teste local;
- um modo completo para os números finais do trabalho.

### 6. OpenMP no build

O `Makefile` atual usa:

```make
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ilibraries
```

Para suportar OpenMP, será necessário acrescentar a flag apropriada, normalmente:

```make
-fopenmp
```

## Ordem recomendada de implementação

1. Resolver a partição em blocos 4/5 sem paralelismo.
2. Implementar os algoritmos candidatos.
3. Implementar a versão sequencial completa.
4. Validar com exemplos pequenos.
5. Implementar a seleção dinâmica por benchmark real.
6. Reaproveitar a mesma lógica para `std::thread`.
7. Reaproveitar a mesma lógica para `OpenMP`.
8. Implementar o benchmark oficial com 6 tamanhos e 30 execuções por tamanho.
9. Ajustar `Makefile` para compilar ambas as versões.
10. Remover o fluxo antigo de benchmark isolado que não serve mais.
11. Gerar os arquivos de resultado para o gráfico.

## Entrega esperada

Ao final, o projeto deve ter:

- uma implementação funcional do algoritmo;
- uma etapa de autoescolha da melhor combinação por tempo medido;
- uma versão sequencial;
- uma versão paralela com `std::thread`;
- uma versão paralela com `OpenMP`;
- uma forma simples de executar;
- um benchmark que siga exatamente os tamanhos e repetições do enunciado;
- um arquivo de resultados com as médias para alimentar o gráfico;
- sem manter caminhos antigos de benchmark que não agregam ao trabalho atual.
