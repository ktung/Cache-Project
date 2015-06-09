/*!
 * \file NUR_strategy.c
 *
 * \brief  Stratégie de remplacement avec NUR..
 * 
 * \author Lucas Soumille
 * \author Pascal Tung
 *
 * $Id: NUR_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"

void Initialize_Flag_R(struct Cache *pcache)
{
    // Si la limite > 0
    if (pcache->nderef > 0)
    {
        // On met le flag REFER à 0 à tous les blocs
        for (int i = 0; i < pcache->nblocks; ++i)
            pcache->headers[i].flags &= ~REFER;


        // Ré initilitialise le compteur
        pcache->pstrategy = 0;

        // Incrémentation du compteur de déréférencement
        ++pcache->instrument.n_deref;
    }
}

int evaluate_RM(struct Cache_Block_Header *bloc)
{
    int rm = 0;
    if ((bloc->flags & REFER) > 0) rm += 2;
    if ((bloc->flags & MODIF) > 0) rm += 1;

    return rm;
}

/*!
 * NUR : Initialisation du compteur
 */
void *Strategy_Create(struct Cache *pcache) 
{
    return pcache->pstrategy = (int*)0;
}

/*!
 * NUR : Rien à faire ici.
 */
void Strategy_Close(struct Cache *pcache)
{
}

/*!
 * NUR : Ré initialisation du compteur et du flag REFER de tous les blocs
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Initialize_Flag_R(pcache);
}

/*! 
 * NUR
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    struct Cache_Block_Header *bloc_NUR = NULL;
    int min_rm = NULL;
    
    /* On cherche d'abord un bloc invalide */
    if ((bloc_NUR = Get_Free_Block(pcache)) != NULL) return bloc_NUR;

    // Pas de bloc libre, on cherche le bloc NUR
    for (int i = 0; i < pcache->nblocks; ++i)
    {
       struct Cache_Block_Header *current = &pcache->headers[i];
       
        // Calcul de rm
        int rm = evaluate_RM(current);
        if (0 == rm)
            return current;
        else
        {
            if (NULL == min_rm || min_rm > rm)
            {
                min_rm = rm;
                bloc_NUR = current;
            }
        }
    }

    return bloc_NUR;
}


/*!
 * NUR
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
    // Si le compteur est >= à la limit, on ré initialize
    if (++pcache->pstrategy >= pcache->nderef)
        Initialize_Flag_R(pcache);

    // On met le flag REFER à 1
    pbh->flags |= REFER;
}  

/*!
 * NUR
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    // Si le compteur est >= à la limit, on ré initialize
    if ((++pcache->pstrategy) >= pcache->nderef)
        Initialize_Flag_R(pcache);

    // On met le flag REFER à 1
    pbh->flags |= REFER;
} 

char *Strategy_Name()
{
    return "NUR";
}
