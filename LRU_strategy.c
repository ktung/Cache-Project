/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement avec LRU.
 * 
 * \author Tùtùre Rapenomanana
 * \author Thibaut Soriano
 *
 * $Id: LRU_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"

/*!
 * LRU : Creation de la Cache_List 
 */
void *Strategy_Create(struct Cache *pcache) 
{
    return Cache_List_Create();
}

/*!
 * LRU : Suppression de la Cache List 
 */
void Strategy_Close(struct Cache *pcache)
{
    Cache_List_Delete(pcache->pstrategy);
}

/*!
 * FIFO : Vide la Cache List.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Cache_List_Clear((struct Cache_List*) pcache->pstrategy);
}

/*! 
 * LRU : Retourne un block free disponible ou supprime le 1er de la Cache_List.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    struct Cache_List *tmpList = (struct Cache_List*)pcache->pstrategy;
    struct Cache_Block_Header *pbh;
    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL){
        Cache_List_Append(tmpList, pbh);
        return pbh;
    }
    struct Cache_Block_Header *header = Cache_List_Remove_First(tmpList);
    Cache_List_Append(tmpList, header);
    return header;
}


/*!
 * LRU : Stratégie seulement sur la date de création.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
        printf("READ %d\n", pbh);

    Cache_List_Move_To_End((struct Cache_List*)pcache->pstrategy, pbh);
}  

/*!
 * LRU : Stratégie seulement sur la date de création.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    printf("WRITE %d\n", pbh);
    Cache_List_Move_To_End((struct Cache_List*)pcache->pstrategy, pbh);
} 

char *Strategy_Name()
{
    return "LRU";
}
