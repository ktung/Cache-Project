#------------------------------------------------------------------
# Un exemple d'implémentation du cache d'un fichier afin d'explorer
# l'effet des algortihmes de gestion et de remplacement
#------------------------------------------------------------------
# Jean-Paul Rigault --- ESSI --- Janvier 2005
#------------------------------------------------------------------
# Makefile
#------------------------------------------------------------------

# Sources

HDR = $(wildcard *.h)
SRC = $(wildcard *.c)

# Exécutables à construire

PROGS = tst_Cache_RAND tst_Cache_FIFO tst_Cache_LRU tst_Cache_NUR

# Fichiers de bibliothèque à reconstruire : initialement vide. 
# Mettre ici les *.o de la bibliothèque que vous avez réimplémentés
# (cache.o low_cache.o cache_list.o)

USRFILES = 

#------------------------------------------------------------------
# Commandes
#------------------------------------------------------------------

# Compilateur et options
CC = gcc
CFLAGS = -std=c99 -Wall -g -D_POSIX_C_SOURCE=200809L
MKDEPEND = $(CC) $(CFLAGS) -MM

# Documentation
DOXYGEN = doxygen

#------------------------------------------------------------------
# Règles par défaut
#------------------------------------------------------------------

# Exécutables avec diverses stratégies
tst_Cache_% : tst_Cache.o %_strategy.o $(USRFILES)	
	$(CC) -o $@ $^ libCache.a

# Exécution des simulations (make simul) avec paramètres par défaut
%_default.out : tst_Cache_%
	$< > $@

#------------------------------------------------------------------
# Cibles principales
#------------------------------------------------------------------

# Exécutables : décommentés les exécutables des stratégies que vous avez implémentées
# N'enlevez pas depend !

all : depend tst_Cache_RAND # tst_Cache_FIFO tst_Cache_LRU tst_Cache_NUR

# Nettoyage 
clean : all
	-rm -f *.o *.out foo

# Nettoyage complet
full_clean :
	-rm -f *.o *.out foo
	-rm -f tst_Cache_RAND tst_Cache_FIFO tst_Cache_LRU tst_Cache_NUR
	-rm depend.out
	-rm -rf Plots

#------------------------------------------------------------------
# Simulation du cache et tracé des courbes
#------------------------------------------------------------------

# Génération des courbes dans le répertoire Plots (voir Makefile.plots)
plots : all
	-mkdir Plots
	cp Makefile.plots Plots/Makefile
	cd Plots; make

#------------------------------------------------------------------
# Reconstruction automatique des dépendances
#------------------------------------------------------------------

depend : 
	$(MAKE) depend.out

depend.out : $(SRC) $(HDR)
	$(MKDEPEND) $(SRC) > depend.out

include depend.out
