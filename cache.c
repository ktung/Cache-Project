/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO Aina Rapenomanana
 */

#include "low_cache.h"

//! Retourne le premier bloc libre ou NULL si le cache est plein
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
	Cache_Block_Header *libre = pcache->pfree;
	pcache->pfree->flags++;
	pcache->pfree = *(pcache->pfree)+1;
	
	return libre;
}
