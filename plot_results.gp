set terminal pngcairo size 1024,768 enhanced font 'Verdana,12'
set output 'results/chart.png'

set title "Comparação de Algoritmos de Ordenação"
set xlabel "Tamanho do Vetor"
set ylabel "Tempo Médio (segundos)"
set grid

set key left top

plot 'results/averages.dat' using 1:2 with linespoints title 'Bubble Sort', \
     'results/averages.dat' using 1:3 with linespoints title 'Insertion Sort', \
     'results/averages.dat' using 1:4 with linespoints title 'Merge Sort', \
     'results/averages.dat' using 1:5 with linespoints title 'Quick Sort', \
     'results/averages.dat' using 1:6 with linespoints title 'Selection Sort'
