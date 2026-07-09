Parte 1 – Faça um programa que implementa o seguinte algoritmo de ordenação. O conjunto de
números deve ser dividido aleatoriamente em blocos de tamanho 4 ou 5, de maneira dinâmica, ou seja,
a cada passo, o algoritmo escolhe aleatoriamente o tamanho do bloco, salva a quantidade de números
correspondente e parte para o próximo bloco. Os blocos devem ser salvos em um conjunto. Use
std::vector ou array para este fim. Uma vez que todos os blocos estejam montados, o algoritmo deve
ordenar cada bloco com a seguinte premissa. Blocos de tamanho par devem ser ordenados com
Algoritmo1. Blocos de tamanho impar devem ser ordenados com Algoritmo2. Uma vez que os blocos
tenham sido ordenados, use o Algoritmo3 para unir os blocos e formar o conjunto com os números
originais ordenados. Faça duas implementações paralelas deste algoritmo. Uma usando threads e uma
usando openmp. O ponto de uso do paralelismo é a ordenação dos blocos. Atenção à combinação de
algoritmos, pois ela pode gerar algoritmos ineficientes.
Possibilidades para Algoritmo1: Selection Sort, Merge Sort, Insertion Sort.
Possibilidade para Algoritmo2: Selection Sort, Merge Sort, Bubble Sort.
Possibilidade para Algoritmo3: Insertion Sort, Merge Sort, Quick Sort.

Monte a combinação mais eficiente para este problema de forma que um algoritmo só pode ser usado
uma vez. Ex: se você escolheu o Insertion Sort como algorittmo1 ele não pode ser usado novamente
como algoritmo3.


Parte 2 - Faça um Relatório com os tempos das duas versões paralelas do algoritmo. Faça também uma
versão sequencial (sem paralelismo). Teste os algoritmos com vetores de tamanhos de múltiplos
tamanhos, começando com 15000 e indo até 20000, aumentando o tamanho de 1000 em 1000. Para
preencher os vetores use números aleatórios no intervalo [0,100000]. Para cada um dos possíveis
tamanhos use 30 vetores com números diferente e calcule a média das 30 execuções.
Para comparação justa, todos os algoritmos devem ordenar os mesmos conjuntos de números.
Gere um Gráfico com as médias de todos os algoritmos. O relatório deve ser escrito no formato de
artigo de congresso com no máximo 12 páginas, ou seja, deve ter introdução, embasamento
teórico, metodologia, resultados conclusões e REFERÊNCIAS. As escolhas dos algoritmos utilizados
devem ser justificadas no relatório. Uma descrição de como foram feitas as versões paralelas também
deve estar presente no relatório.