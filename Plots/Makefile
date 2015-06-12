#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
# $Id: Makefile.plots,v 1.2 2008/02/29 16:21:12 jpr Exp $
#------------------------------------------------------------------
# Makefile secondaire, déroulant des tests de variation de paramètres
# avec tracé de courbe.
#------------------------------------------------------------------

SCRIPTS = ..

all plots : cache_size read_write_ratio seq_access working_sets locality
	for eps in *.eps; do \
		convert $$eps `basename $$eps .eps`.jpg; \
	done

cache_size :
	for t in 1 2 3 4 5 ; do \
		$(SCRIPTS)/plot.sh -T "Effet de la taille du cache" \
			-o t$$t-cache_size -x "taille fichier / taille cache" \
			-t $$t -L "20,70" \
			-- -r 1000 750 500 400 300 200 100 50 10 5 1 ; \
		$(SCRIPTS)/plot.sh -T "Effet de la taille du cache zoomé" \
			-o t$$t-cache_size-zoom -x "taille fichier / taille cache" \
			-t $$t -L "20,70" \
			-- -r 180 160 140 120 100 50 30 10 5 2 1 ; \
		$(SCRIPTS)/cache_size-inv.sh t$$t-cache_size $t ; \
		$(SCRIPTS)/cache_size-inv.sh t$$t-cache_size-zoom $t ; \
	done

read_write_ratio :
	for t in 3 4 5; do \
		$(SCRIPTS)/plot.sh -T "Effet du ratio lectures / écritures" \
			-o t$$t-rw_ratio -x "Ratio lectures / écritures" \
			-t $$t -L "20,70" \
			-- -w 1 2 5 10 30 50 80 100 150 200 300 400 500 ; \
	done

nloops :
	for t in 2 3 4 5; do \
		$(SCRIPTS)/plot.sh -T "Effet du nombre d'itérations" \
			-o t$$t-nloops -x "Facteur du nombre d'itérations" \
			-t $$t -L "10,85" \
			-- -L 1 2 3 4 5 6 7 8 9 10 15 20 30 40 50 ; \
	done

seq_access :
	$(SCRIPTS)/plot.sh -T "Effet du nombre de blocs séquentiels locaux" \
		-o t3-seq_access -x "Nombre maximum de blocs locaux" \
		-t 3 -L "50,40" \
		-- -s 1 2 3 4 5 6 7 8 9 10 15 20  50 100 200 300 500

working_sets :
	for t in 4 5; do \
		$(SCRIPTS)/plot.sh -T "Effet du nombre de Working Sets" \
			-o t$$t-working_sets -x "Nombre de Working Sets" \
			-t $$t -L "50,80" \
			-- -W 1 2 5 10 30 50 80 100 150 200 300 400 500 ; \
		$(SCRIPTS)/plot.sh -T "Effet du nombre de Working Sets zoomé" \
			-o t$$t-working_sets-zoom -x "Nombre de Working Sets" \
			-t $$t -L "20,90" \
			-- -W 1 2 5 10 30 50 80 100 ; \
	done

locality : 
	for t in 4 5; do \
		$(SCRIPTS)/plot.sh -T "Effet de la localité" \
			-o t$$t-locality -x "Largeur de la fenêtre de localité" \
			-t $$t -L "100,70" \
			-- -L 5 10 20 50 100 150 200 250 300 350 400 500 600 ; \
	done

