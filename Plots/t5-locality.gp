set style data linespoints
set xlabel "Largeur de la fenêtre de localité"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la localité (test 5)" font "Helvetica-Bold,18" at 100,70 
set encoding utf8
set terminal postscript eps color
set output "t5-locality.eps"
plot "t5-locality" using 1:2 t "NUR", "t5-locality" using 1:3 t "LRU", "t5-locality" using 1:4 t "FIFO", "t5-locality" using 1:5 t "RAND"
