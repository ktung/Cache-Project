/*!
 * \file low_cache.h
 *
 * \brief  Programme de test du cache
 * 
 * \author Jean-Paul Rigault 
 */


#include <stdio.h>

#include "cache.h"
#include "strategy.h"
#include "random.h"
#include "stdbool.h"

/* ------------------------------------------------------------------------------------
 * Valeurs par défaut des variables globales qui suivent 
 * ------------------------------------------------------------------------------------
 */

#define DEF_FILE "foo"		/* Nom du fichier */
#define N_RECORDS_PER_BLOCK 10	/* Nombre d'enregistrements par bloc */	
#define N_RECORDS_IN_FILE 30000	/* Nombre total d'enregistrements dans le fichier */
#define RATIO_FILE_CACHE 100	/* Ratio taille fichier / taille cache */
#define N_LOOPS 3		/* Nombre total d'accès (lecture ou écriture) */
#define RATIO_READ_WRITE 10	/* Rapport nombre de lectures / nombre d'écritures */
#define N_SEQ_ACCESS 5		/* Nombre maximum d'accès séquentiels */
#define N_WORKING_SETS 100	/* Nombre de "working sets" */
#define N_LOCAL_WINDOW 300	/* Largeur de la "fenêtre de localité" */
#define N_DEREF 100		/* Période de déréférençage (stratégie NUR) */

/* ------------------------------------------------------------------------------------
 * Variables globales 
 * ------------------------------------------------------------------------------------
*/

/* Configuration du cache
 * ----------------------
 */

/* Fichier par défaut */
char *File = DEF_FILE;

/* Nombre d'enregistrements par bloc */			
unsigned int N_Records_per_Block = N_RECORDS_PER_BLOCK;

/* Nombre total d'enregistrements dans le fichier */
int N_Records_in_File = N_RECORDS_IN_FILE;

/* Ratio taille fichier / taille cache */		
int Ratio_File_Cache = RATIO_FILE_CACHE;

/* Nb de blocs dans le cache
 * Le cache comportera N_Records_in_File / Ratio_File_Cache enregistrements
 */	   
unsigned int N_Blocks_in_Cache;

/* Configuration des tests
 * -----------------------
 */

/* Nombre total d'accès (lecture ou écriture) pour les tests 2, 3 et 4
 * En fait, le nombre d'accès sera N_Loops * N_Records_in_File
 */		
int N_Loops = N_LOOPS;

/* Rapport nombre de lectures / nombre d'écritures
 * Pour les tests 3 et 4
*/	
int Ratio_Read_Write = RATIO_READ_WRITE;

/* Nombre maximum d'accès séquentiels
 * pour le test 3
 */
int N_Seq_Access = N_SEQ_ACCESS;

/* Nombre de "working sets"
 * Pour test 4
 */
int N_Working_Sets = N_WORKING_SETS;

/* Largeur de la "fenêtre de localité"
 * Pour test 4
 */
int N_Local_Window = N_LOCAL_WINDOW;

/* Période de déréférençage pour la stratégie NUR seulement */
int N_Deref = N_DEREF;

/* Format de sortie court */
int Short_Output = 0;

/* Une structure quelconque pour les enregistrements du cache
 * ----------------------------------------------------------
 */
struct Any
{
    int i;
    double x;
};

/* Taille (en bytes) d'un bloc utilisateur */
int Record_Size = sizeof(struct Any);

/* Pointeur sur le cache
 * ---------------------
 */
struct Cache *The_Cache;

/* ------------------------------------------------------------------------------------
 * Prototypes de fonctions définies plus tard 
 * ------------------------------------------------------------------------------------
*/

/* Messages d'erreur */
static void Error(const char *msg);
static void Usage(const char *execname);

/* Impression des résultats */
static void Print_Parameters();
static void Print_Instrument(struct Cache *pcache, const char *msg);

/* Décodage des paramètres */
static void Scan_Args(int argc, char *argv[]);

/* Tests individuels */
static void Test_1();
static void Test_2();
static void Test_3();
static void Test_4();
static void Test_5();

static void (*Tests[])() = {
    Test_1,
    Test_2,
    Test_3,
    Test_4,
    Test_5,
};
#define NTESTS ((int)(sizeof(Tests)/sizeof(Tests[0])))

/* Tests sélectionnés */
static int Do_Test[NTESTS] = {false, false, false, false, false};

/* ------------------------------------------------------------------------------------
 * Programme principal
 * -------------------
 * Décodage des arguments,
 * Création et initialisation le cache, 
 * Exécution des tests
 * Fermeture (et destruction) du cache.
 * ------------------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
    int i;

    /* Décodage des arguments de la ligne de commande */
    Scan_Args(argc, argv);

    /* Initialisation du cache */
    if ((The_Cache = Cache_Create(File, N_Blocks_in_Cache, N_Records_per_Block,
                                  Record_Size, N_Deref)) == NULL)
	Error("Cache_Init");
    Print_Parameters();

    /* Exécution des tests */
    for (i = 0; i < NTESTS; ++i)
    {
        if (Do_Test[i]) Tests[i]();
    }

    /* Fermeture du cache */
    if (!Cache_Close(The_Cache)) Error("Cache_Close");

    return 0;
}

/* ------------------------------------------------------------------------------------ 
 * Les différents tests
 * --------------------
 *
 * Tous les tests commencent par invalider le cache, déroulent une boucle de
 * lecture et/ou d'écriture, puis affichent le résultat de l'instrumentation.
 *
 * Dans ces tests, ind désigne l'indice-fichier de l'enregistrement à lire ou
 * écrire.
 * ------------------------------------------------------------------------------------
*/

/* Test 1 : boucle de lecture séquentielle
 * ---------------------------------------

 * On écrit un enregistreùent et on lit le précédent. La localité est bien sûr
 * excellente, toutes les stratégies doivent être efficaces.
*/
static void Test_1()
{
    int ind;	/* indice-fichier de l'enregistrement à écrire */ 
    struct Any temp = {0, 0.0};

    if (!Cache_Invalidate(The_Cache)) Error("Test_1 : Cache_Invalidate");

    if (!Cache_Write(The_Cache, 0, &temp)) Error("Test_1 : Cache_Write(0)");
    for (ind = 1; ind < N_Records_in_File; ind++)
    {
        temp.i = ind;
        temp.x = (double)ind;
	if (!Cache_Write(The_Cache, ind, &temp)) Error("Test_1 : Cache_Write");
	if (!Cache_Read(The_Cache, ind - 1, &temp)) Error("Test_1 : Cache_Read");
    }

    Print_Instrument(The_Cache, "Test_1 : boucle de lecture séquentielle");
}

/* Test 2 : boucle d'écriture aléatoire
 * ------------------------------------
 
 * On tire au sort ind entre 0 et N_Records_in_File et on écrit
 * l'enregistrement correspondant.
*/
void Test_2()
{
    int ind;	/* indice-fichier de l'enregistrement à écrire */

    /* Invalidation du cache */
    if (!Cache_Invalidate(The_Cache)) Error("Test_2 : Cache_Invalidate");

    /* Accès (purement) aléatoire aux éléments en écriture */
    for (ind = 0; ind < N_Loops; ind++)
    {
	int ind = RANDOM(0, N_Records_in_File);
        struct Any temp;

        temp.i = ind;
        temp.x = (double)ind;
	if (!Cache_Write(The_Cache, ind, &temp)) Error("Test_2 : Cache_Write");
    }

    Print_Instrument(The_Cache, "Test_2 : boucle écriture aléatoire");
}

/* Test 3 : boucle de lecture/écriture aléatoire
 * ---------------------------------------------

 * On tire au sort l'indice de l'enregistrement ainsi que le nombre nr
 * d'enregistrements consécutifs à partir de ind (nr est compris entre 1 et
 * N_Seq_Access - 1). Tous les Ratio_Read_Write accès on fait une écriture,
 * sinon une lecture. 
*/
void Test_3()
{
    int i;

    /* Invalidation du cache */
    if (!Cache_Invalidate(The_Cache)) Error("Test_3 : Cache_Invalidate");

    /* Boucle de lecture/écriture aléatoire */
    for (i = 0; i < N_Loops; )
    {
	int ind = RANDOM(0, N_Records_in_File);	/* indice-fichier de l'enregistrement */
        int nr = RANDOM(1, N_Seq_Access);	/* nombre d'enregistrements consécutifs */
        int j;

        if (nr <= 0) nr = 1;

        /* On lit ou écrit nr blocs consécutifs à partir de ind */
        for (j = 0; j < nr; ++j)
        {
            struct Any temp;
            /* On fait une écriture tous les Ratio_Read_Write accès */
            int rd = ((i + j) % Ratio_Read_Write != 0);

            temp.i = ind;
            temp.x = (double)ind;  
            if (rd)
            {
                if (!Cache_Read(The_Cache, ind + j, &temp)) 
                    Error("Test_3 : Cache_Read");
            }
            else
            {
                if (!Cache_Write(The_Cache, ind + j, &temp)) 
                    Error("Test_3 : Cache_Write");
            }
	}
        /* On a fait nr accès de plus */
        i += nr;
    }

    Print_Instrument(The_Cache, "Test_3 : boucle lecture/écriture aléatoire");
}

/* Test 4 : boucle de lecture/écriture aléatoire avec localité
 * -----------------------------------------------------------

 * Ce test tente de simuler une certaine localité. On effectue en fait
 * N_Working_Sets phases, chaque phase comportant le même nombre d'accès (noté
 * nlocal). Pour chaque phase, on tire au sort un enregistrement de base (ind),
 * puis on tire au sort (nlocal fois) un incrément (incr) afin d'accéder à
 * l'enregistrement ind + incr.
 *
 * Comme dans le test précédent on effectue une écriture tous les
 * Ratio_Read_Write accès, une lecture sinon.
*/
void Test_4()
{
    int nlocal = N_Loops / N_Working_Sets; /* nombre d'accès locaux pour un working set */
    int i;

    /* Invalidation du cache */
    if (!Cache_Invalidate(The_Cache)) Error("Test_4 : Cache_Invalidate");

    /* Boucle mixte de lecture/écriture aléatoire */

    for (i = 0; i < N_Loops; i += nlocal)
    {
	int ind = RANDOM(0, N_Records_in_File);	/* indice-fichier de
                                                 * l'enregistrement de base */
        int j;

        /* On effectue naccess accès à partir de ind */
        for (j = 0; j < nlocal; ++j)
        {
            int incr = RANDOM(0, N_Local_Window);/* on tire au sort
                                                  * l'enregistrement dans la
                                                  * fenêtre locale */           
            int ind1 = ind + incr;		/* indice de l'enregistrement à accéder */
            /* On fait une écriture tous les Ratio_Read_Write accès */
            int rd = (ind1 % Ratio_Read_Write != 0);
            struct Any temp;

            temp.i = ind1;
            temp.x = (double)ind1;  

            if (rd)
            {
                if (!Cache_Read(The_Cache, ind1, &temp)) 
                    Error("Test_4 : Cache_Read(ind)");
            }
            else
            {
                if (!Cache_Write(The_Cache, ind1, &temp)) 
                    Error("Test_4 : Cache_Write(ind)");
            }
	}
     }

    Print_Instrument(The_Cache, "Test_4 : boucle lecture/écriture aléatoire avec localité");
}
/* Test 5 : boucle de lecture/écriture séquentielle avec localité
 * --------------------------------------------------------------

 * Ce test tente de combiner une certaine localité avec la sequentialité. On effectue en fait
 * N_Working_Sets phases, chaque phase comportant le même nombre d'accès (noté
 * nlocal). Pour chaque phase, l'enregistrement de base (ind) progresse séquentiellement,
 * puis on tire au sort (nlocal fois) un incrément (incr) afin d'accéder à
 * l'enregistrement ind - incr (c'est-à-dire quelque chose qui a été accédé précedemment).
 *
 * Comme dans le test précédent on effectue une écriture tous les
 * Ratio_Read_Write accès, une lecture sinon.
*/
void Test_5()
{
    /* nombre d'accès locaux pour un working set */
    int nlocal = N_Loops / N_Working_Sets; 
    int i;
    int k;

    /* Invalidation du cache */
    if (!Cache_Invalidate(The_Cache)) Error("Test_5 : Cache_Invalidate");

    /* Boucle mixte de lecture:écriture aléatoire */

    for (i = 0, k = 0; i < N_Loops; i += nlocal, ++k)
    {
        if (i % N_Working_Sets == 0)
        {
            /* indice-fichier de l'enregistrement de base */
            int ind = k * N_Records_in_File / N_Working_Sets;
            int j;

            /* On effectue nlocal accès à partir de ind */
            for (j = 0; j < nlocal; ++j)
            {
                int incr = RANDOM(0, N_Local_Window);/* on tire au sort
                                                      * l'enregistrement dans la
                                                      * fenêtre locale */           
                int ind1 = ind - incr;     	/* indice de l'enregistrement à accéder */
                /* On fait une écriture tous les Ratio_Read_Write accès */
                int rd = (ind1 % Ratio_Read_Write != 0);
                struct Any temp;

                if (ind1 < 0) ind1 = 0;
                if (ind1 >= N_Records_in_File) ind1 = N_Records_in_File -1;

                temp.i = ind1;
                temp.x = (double)ind1;  

                if (rd)
                {
                    if (!Cache_Read(The_Cache, ind1, &temp)) 
                        Error("Test_5 : Cache_Read(ind)");
                }
                else
                {
                    if (!Cache_Write(The_Cache, ind1, &temp)) 
                        Error("Test_5 : Cache_Write(ind)");
                }
            }
        }
    }

    Print_Instrument(The_Cache,
                     "Test_5 : boucle lecture/écriture séquentielle avec localité");
}
	
/* ------------------------------------------------------------------------------------
 * Fonctions locales (privées) à ce module
 * ------------------------------------------------------------------------------------
*/

/* Fonction d'affichage d'erreur 
 * -----------------------------
 */
static void Error(const char *msg)
{
    fprintf(stderr, "ERROR *** %s\n", msg);
    exit(1);
}

/* Fonction d'affichage des paramètres généraux du cache
 * -----------------------------------------------------
*/
static void Print_Parameters()
{
    int recordsz = sizeof(struct Any);
    int blocksz = N_Records_per_Block * recordsz;
    int cachesz = N_Blocks_in_Cache * blocksz;
    int filesz = N_Records_in_File * recordsz;

    if (Short_Output)
    {
        printf("%s\nrecsz %d\nnrec %d\nnblk %d\nnrecblk %d\n", Strategy_Name(),
               recordsz, N_Records_in_File, N_Blocks_in_Cache, N_Records_per_Block);          
        printf("file/cache %.2f\nnloop %d\nrw %d\n", 100 * (double)cachesz / filesz,
               N_Loops, Ratio_Read_Write);
        printf("nseq %d\nnws %d\nnloc %d\nnderef %d\n", N_Seq_Access, N_Working_Sets,
               N_Local_Window, N_Deref);
    }
    else
    {
        printf("================ Configuration du cache ================\n");
        printf("Paramètres du fichier :\n");
        printf("\t%d enregistrements %d octets totaux\n", N_Records_in_File, filesz);

        printf("Paramètres du cache :\n");
        printf("\t%d blocs %d enregistrements/bloc %d octets/enregistrement\n", 
               N_Blocks_in_Cache, N_Records_per_Block, recordsz);
        printf("\t%d octets/bloc %d octets totaux\n", blocksz, cachesz);
        printf("\tRapport cache/fichier : %.2f %%\n", 100 * (double)cachesz / filesz);
        printf("\tStratégie : %s\n", Strategy_Name());

        printf("Paramètres des tests :\n");
        printf("\tNombre d'accès : %d\n", N_Loops);
        printf("\tRapport lectures/écritures : %d\n", Ratio_Read_Write);
        printf("\tNombre maximum accès séquentiels : %d\n", N_Seq_Access);
        printf("\tNombre de Working Sets : %d\n", N_Working_Sets);  
        printf("\tLargeur de la fenêtre de localité : %d\n", N_Local_Window);
        printf("\tFréquence de déréférençage pour NUR : %d\n", N_Deref);
        printf("========================================================\n");
    }
}

/* Recupération des données d'instrumentation
 *  ------------------------------------------
 */
static void Print_Instrument(struct Cache *pcache, const char *msg)
{
    struct Cache_Instrument *pinstr = Cache_Get_Instrument(pcache);

    if (Short_Output)
    {
        printf("hits %.1f\n", 
               ((double)pinstr->n_hits)/(pinstr->n_reads + pinstr->n_writes)*100);
    }
    else
    {
        printf("\n%s : \n", msg == NULL ? "" : msg);
        printf("\t%d lectures %d écritures %d succès (%.1f %%)\n",
               pinstr->n_reads, pinstr->n_writes, pinstr->n_hits, 
               ((double)pinstr->n_hits)/(pinstr->n_reads + pinstr->n_writes)*100);
        printf("\t%d syncs %d déréférençages\n", pinstr->n_syncs, pinstr->n_deref);
    }
}

/* Information d'utilisation
 * -------------------------
*/
static void Usage(const char *execname)
{
    printf("\nUsage: %s [options]\n", execname);
    printf("\nOptions générales\n"
           "-----------------\n"
           "-h\tce message\n"
           "-p\taffiche les paramètres du cache sans exécuter de test\n"
           "-S\tformat de sortie court");
    printf("\nOptions de configuration du cache\n"
           "---------------------------------\n"
           "-f file\tnom du fichier\n"
           "-N nr\tnombre d'enregistrements dans le fichier\n"
           "-R nrb\tnombre d'enregistrements par bloc du cache\n" 
           "-r rfc\trapport taille fichier / taille cache\n");
    printf("\nOptions de configuration des tests\n"
           "----------------------------------\n"
           "-t nt\tactive le test nt ; il peut y avoir plusieurs options -t\n"
           "\t(nt de 1 à %d)\n", NTESTS);
    printf("-l nl\tle nombre d'accès dans les tests 2 à 4 sera 'nl * nr'\n"
           "-w rwr\trapport nombre lectures / nombre écritures (tests 3 et 4)\n"
           "-s ns\tnombre de blocs à lire séquentiellement (test 3)\n"
           "-W nws\tnombre de working sets (tests 4 et 5)\n"
           "-L nl\tlongueur de la fenêtre de localité (test 4 et 5)\n"
           "-d dr\tpériode de déréférençage pour NUR\n");
}
    
/* Analyse des arguments 
 * ----------------------
 */
static void Scan_Args(int argc, char *argv[])
{
    int i;
    int ntests = 0;
    int just_print = 0;
    int numtest;

    for (i = 1; i < argc; i++)
    {
	if (argv[i][0] == '-')
	{
	    switch (argv[i][1])
	    {
                /* Options générales */

	    case 'h':
		Usage(argv[0]);
		exit(1);
            case 'p':
                just_print = 1;
                break;
            case 'S':
                Short_Output = 1;
                break;

                /* Options de configuration du cache */

	    case 'f':
		File = argv[++i];
		break;	    
	    case 'N':
		N_Records_in_File = atoi(argv[++i]);
		break;
	    case 'R':
		N_Records_per_Block = atoi(argv[++i]);
		break;
	    case 'r':
		Ratio_File_Cache = atoi(argv[++i]);
		break;

                /* Options de configuration des tests */

            case 't':
		numtest = atoi(argv[++i]);
                if (numtest >= 1 && numtest <= NTESTS) 
                {
                    Do_Test[numtest - 1] = 1;
                    ++ntests;
                }
                else 
                {
                    Usage(argv[0]);
                    exit(1);
                }
		break;
	    case 'l':
		N_Loops = atoi(argv[++i]);
		break;
	    case 'w':
		Ratio_Read_Write = atoi(argv[++i]);
		break;
            case 's':
                N_Seq_Access = atoi(argv[++i]);
		break;          
            case 'W':
                N_Working_Sets = atoi(argv[++i]);
		break;            
            case 'L':
                N_Local_Window = atoi(argv[++i]);
		break;
            case 'd':
                N_Deref = atoi(argv[++i]);
                break;
	    default:
		fprintf(stderr, "Option inconnue : %s\n", argv[i]);
		exit(1);
	    }
	}
	else
	{
	    Usage(argv[0]);
	    exit(1);
	}
    }

    N_Loops = N_Loops * N_Records_in_File;
    N_Blocks_in_Cache = N_Records_in_File / N_Records_per_Block / Ratio_File_Cache;

    if (just_print)
    {
        Print_Parameters();
        exit(0);
    }

    if (ntests == 0)
    {
        // Aucun test choisi : on les active tous
        for (i = 0; i < NTESTS; ++i) 
            Do_Test[i] = true;
    }
}

