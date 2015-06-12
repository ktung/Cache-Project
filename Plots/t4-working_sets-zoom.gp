set style data linespoints
set xlabel "Nombre de Working Sets"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet du nombre de Working Sets zoomé (test 4)" font "Helvetica-Bold,18" at 20,90 
set encoding utf8
set terminal postscript eps color
set output "t4-working_sets-zoom.eps"
plot "t4-working_sets-zoom" using 1:2 t "NUR", "t4-working_sets-zoom" using 1:3 t "LRU", "t4-working_sets-zoom" using 1:4 t "FIFO", "t4-working_sets-zoom" using 1:5 t "RAND"
