/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO
 * \author Natiora Rapenomanana
 */

#include "low_cache.h"

//! Retourne le premier bloc libre ou NULL si le cache est plein
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
	Cache_Block_Header *libre = pcache->pfree;
	pcache->pfree->flags |= VALID;
	// On parcours tous les autres block restante pour trouver un bloc 'free'
	for(int i = 0; i < pcache->nblocks; i++) {
		if((pcache->headers[i]->flags & VALID)==0) {
			pcache->pfree = pcache->headers[i];
			break;
		}
	}
	
	return libre;
}
