
/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO
 * \author Natiora Rapenomanana
 */

#include "low_cache.h"
#include "cache.h"

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

//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){
	struct Cache * cache = malloc(sizeof(struct Cache));
	cache->file = fic;
	cache->fp = fopen(fic, "r+");
	cache->nblocks = nblocks;
	cache->nderef = nderef;
	cache->blocksz = recordsz * nrecords;
	cache->nrecords = nrecords;
	cache->recordsz = recordsz;
	cache->pstrategy = NULL;
	struct Cache_Instrument instr = malloc(sizeof(struct Cache_Instrument));
	cache->instrument = instr;
	struct Cache_Block_Header * headers = malloc(sizeof(struct Cache_Block_Header) * nblocks);
	cache->headers = headers;
	cache->pfree = Get_Free_Block(cache);

	return cache;
}
