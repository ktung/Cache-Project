set style data linespoints
set xlabel "Ratio lectures / écritures"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet du ratio lectures / écritures (test 5)" font "Helvetica-Bold,18" at 20,70 
set encoding utf8
set terminal postscript eps color
set output "t5-rw_ratio.eps"
plot "t5-rw_ratio" using 1:2 t "NUR", "t5-rw_ratio" using 1:3 t "LRU", "t5-rw_ratio" using 1:4 t "FIFO", "t5-rw_ratio" using 1:5 t "RAND"
