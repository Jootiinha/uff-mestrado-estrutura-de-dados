set terminal pngcairo size 1024,768 enhanced font 'Verdana,12'
set output 'results/chart.png'

set title "Comparacao das Versoes do Algoritmo por Blocos"
set xlabel "Tamanho do Vetor"
set ylabel "Tempo Medio (segundos)"
set grid
set key left top

plot 'results/averages.dat' using 1:2 with linespoints linewidth 2 title 'Sequencial', \
     'results/averages.dat' using 1:3 with linespoints linewidth 2 title 'std::thread', \
     'results/averages.dat' using 1:4 with linespoints linewidth 2 title 'OpenMP'
