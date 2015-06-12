set style data linespoints
set xlabel "Nombre maximum de blocs locaux"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet du nombre de blocs séquentiels locaux (test 3)" font "Helvetica-Bold,18" at 50,40 
set encoding utf8
set terminal postscript eps color
set output "t3-seq_access.eps"
plot "t3-seq_access" using 1:2 t "NUR", "t3-seq_access" using 1:3 t "LRU", "t3-seq_access" using 1:4 t "FIFO", "t3-seq_access" using 1:5 t "RAND"
