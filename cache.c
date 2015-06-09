
/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO Aina Rapenomanana
 */

#include "low_cache.h"
#include "cache.c"

//! Retourne le premier bloc libre ou NULL si le cache est plein
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
	Cache_Block_Header *libre = pcache->pfree;
	pcache->pfree->flags++;
	pcache->pfree = *(pcache->pfree)+1;
	
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
