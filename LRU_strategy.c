/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement avec LRU..
 * 
 * \author Jean-Paul Rigault
 *
 * $Id: LRU_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"
#include "time.h"
#include "cache_list.h"

/*!
 * LRU 
 *
 * En fait, nous initialisons le germe
 * (seed) du générateur aléatoire à quelque chose d'éminemment variable, pour
 * éviter d'avoir la même séquence à chque exécution...
 */
void *Strategy_Create(struct Cache *pcache) 
{
    return Cache_List_Create();
}

/*!
 * LRU : Rien à faire ici.
 */
void Strategy_Close(struct Cache *pcache)
{
	Cache_List_Delete(pcache->pstrategy);
}

/*!
 * LRU : Rien à faire ici.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    Cache_List_Clear((struct Cache_List*) pcache->pstrategy);
}

/*! 
 * LRU : On prend le premier bloc invalide. S'il n'y en a plus, on prend un bloc au hasard.
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
 * LRU : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
	struct Cache_List *tmpList = (struct Cache_List*)pcache->pstrategy;
	struct Cache_Block_Header *header = Cache_List_Remove(tmpList, pbh);
	Cache_List_Append(tmpList, header);
}  

/*!
 * LRU : Rien à faire ici.
 */  
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
	struct Cache_List *tmpList = (struct Cache_List*)pcache->pstrategy;
	struct Cache_Block_Header *header = Cache_List_Remove(tmpList, pbh);
	Cache_List_Append(tmpList, header);
} 

char *Strategy_Name()
{
    return "LRU";
}
