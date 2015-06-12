set style data linespoints
set xlabel "Nombre de Working Sets"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet du nombre de Working Sets (test 5)" font "Helvetica-Bold,18" at 50,80 
set encoding utf8
set terminal postscript eps color
set output "t5-working_sets.eps"
plot "t5-working_sets" using 1:2 t "NUR", "t5-working_sets" using 1:3 t "LRU", "t5-working_sets" using 1:4 t "FIFO", "t5-working_sets" using 1:5 t "RAND"
