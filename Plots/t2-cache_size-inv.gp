set style data linespoints
set xlabel "taille cache / taille fichier (%)"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la taille du cache (test )" font "Helvetica-Bold,18" at 400,70
set encoding utf8
set terminal postscript eps color
set output "t2-cache_size-inv.eps"
plot "t2-cache_size-inv" using 1:2 t "NUR", "t2-cache_size-inv" using 1:3 t "LRU", "t2-cache_size-inv" using 1:4 t "FIFO", "t2-cache_size-inv" using 1:5 t "RAND"
