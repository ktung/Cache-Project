#! /bin/sh

#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: cache_size-inv.sh,v 1.2 2008/02/29 16:21:12 jpr Exp $
#------------------------------------------------------------------
# Scripts de tracé du taux de succès en fonction du rapport 
# taille cache / taille fichier (inverse de l'option -r des
# programmes de test
#------------------------------------------------------------------

file=$1
Out=${file}-inv
t=$2

awk '{printf "%.2f", 100.0/$1; print $0}' $file > $Out

ed - << EOF
a
set style data linespoints
set xlabel "taille cache / taille fichier (%)"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "Effet de la taille du cache (test $t)" font "Helvetica-Bold,18" at 400,70
set encoding utf8
set terminal postscript eps color
set output "$Out.eps"
plot "$Out" using 1:2 t "NUR", "$Out" using 1:3 t "LRU", "$Out" using 1:4 t "FIFO", "$Out" using 1:5 t "RAND"
.
w $Out.gp
q
EOF

gnuplot $Out.gp
