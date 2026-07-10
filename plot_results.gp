set terminal pngcairo size 1024,768 enhanced font 'Verdana,12'
set xlabel "Tamanho do Vetor da Execucao Principal"
set grid
set key left top

set output 'results/chart_selection.png'
set title "Custo Total da Selecao Dinamica"
set ylabel "Tempo Total (segundos)"
plot 'results/averages.dat' using 1:2 with linespoints linewidth 2 title 'Ranking completo'

set output 'results/chart_comparison.png'
set title "Comparacao das Versoes do Algoritmo por Blocos"
set ylabel "Tempo Medio (segundos)"
plot 'results/averages.dat' using 1:7 with linespoints linewidth 2 title 'Sequencial', \
     'results/averages.dat' using 1:8 with linespoints linewidth 2 title 'std::thread', \
     'results/averages.dat' using 1:9 with linespoints linewidth 2 title 'OpenMP'

set output 'results/chart_selection_ranking.png'
set title "Cinco Melhores Combinacoes na Amostra de Selecao"
set ylabel "Tempo Medio (segundos)"
plot 'results/averages.dat' using 1:10 with linespoints linewidth 2 title '1o lugar', \
     'results/averages.dat' using 1:11 with linespoints linewidth 2 title '2o lugar', \
     'results/averages.dat' using 1:12 with linespoints linewidth 2 title '3o lugar', \
     'results/averages.dat' using 1:13 with linespoints linewidth 2 title '4o lugar', \
     'results/averages.dat' using 1:14 with linespoints linewidth 2 title '5o lugar'

set output 'results/chart_selection_gain.png'
set title "Ganho da Selecao Dinamica contra Combinacao Fixa"
set ylabel "Reducao de Tempo na Validacao (%)"
set yzeroaxis linewidth 1
set boxwidth 600
set style fill solid 0.5
unset key
plot 'results/averages.dat' using 1:6 with boxes

set output 'results/chart_selection_choices.png'
set title "Algoritmos Escolhidos em Cada Etapa"
set ylabel "Algoritmo Escolhido"
set key left top
unset yzeroaxis
set yrange [0.5:5.5]
set ytics ("InsertionSort" 1, "SelectionSort" 2, "BubbleSort" 3, "MergeSort" 4, "QuickSort" 5)
plot 'results/averages.dat' using 1:15 with points pointtype 7 pointsize 1.5 title 'Blocos pares', \
     'results/averages.dat' using 1:16 with points pointtype 5 pointsize 1.5 title 'Blocos impares', \
     'results/averages.dat' using 1:17 with points pointtype 9 pointsize 1.5 title 'Etapa final'
