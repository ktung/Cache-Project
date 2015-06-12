set style data linespoints
set xlabel "taille fichier / taille cache"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la taille du cache (test 5)" font "Helvetica-Bold,18" at 20,70 
set encoding utf8
set terminal postscript eps color
set output "t5-cache_size.eps"
plot "t5-cache_size" using 1:2 t "NUR", "t5-cache_size" using 1:3 t "LRU", "t5-cache_size" using 1:4 t "FIFO", "t5-cache_size" using 1:5 t "RAND"
