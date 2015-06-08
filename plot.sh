#! /bin/sh

#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: plot.sh-gnuplot-v4,v 1.1.1.1 2005/03/22 11:44:19 jpr Exp $
#------------------------------------------------------------------
# Tracé des courbes comparant les différentes stratégies en fonction
# des différents paramètres du cache.
#
# Attention : Cette version ne marche qu'avec gnuplot v4.
#------------------------------------------------------------------

if [ $# -le 7 ]
then
    echo "Usage:" 2>&1
    echo "plot.sh -i \\" 2>&1
    echo "        -o nom_base_fichier_sortie \\" 2>&1
    echo "        -T titre \\" 2>&1
    echo "        -x étiquette_axe_x \\" 2>&1
    echo "        -t numéro_test \\" 2>&1
    echo "        -L "x,y" (position du titre)\\" 2>&1
    echo "        -- option_test valeur..." 2>&1  
    exit 1
fi

Interactive=""
Title=
OutputFile=out_$$
XTitle=
LabelPos="20,70"
SimulOpt=
TestNum=5

for arg in $*
do
    case $1 in
    -i) Interactive="-"; shift;;
    -T) Title="$2"; shift 2;;
    -o) Out="$2"; shift 2;;
    -x) XTitle="$2"; shift 2;;
    -t) TestNum="$2"; shift 2;;
    -L) LabelPos="$2"; shift 2;; 
    --) shift; break;;
    esac
done
SimulOpt="$1"; shift

echo Titre : $Title
echo Fichier : $Out
echo Axe des X : $XTitle
echo Numéro de test : $TestNum
echo Position du titre : $LabelPos
echo Option de simulation : $SimulOpt
echo Valeurs : $*

rm -f $Out.gp $Out 

for n in $*
do
    echo -n $n
    for strategy in NUR LRU FIFO RAND
    do 
        ../tst_Cache_$strategy -t $TestNum $SimulOpt $n -S | \
            awk '$1 ~ /hits/ {printf " %.2f ", $2}' 
    done
    echo
done > $Out

ed - << EOF
a
set style data linespoints
set xlabel "$XTitle"
set ylabel "Hit rate (%)" font "Helvetica-Oblique"
set label "$Title (test $TestNum)" font "Helvetica-Bold,18" at $LabelPos 
set encoding utf8
set terminal postscript eps color
set output "$Out.eps"
plot "$Out" using 1:2 t "NUR", "$Out" using 1:3 t "LRU", "$Out" using 1:4 t "FIFO", "$Out" using 1:5 t "RAND"
.
w $Out.gp
q
EOF

if [ -n "$Interactive" ]
then
    ed - $Out.gp << EOF
a
set terminal x11
plot "$Out" using 1:2 t "NUR", "$Out" using 1:3 t "LRU", "$Out" using 1:4 t "FIFO", "$Out" using 1:5 t "RAND"
.
w
q
EOF
fi

gnuplot $Out.gp $Interactive

exit 0
