
/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO
 * \author Natiora Rapenomanana
 * \contributors Lucas SOUMILLE Pascal TUNG
 */

#include "low_cache.h"
#include "cache.h"
#include "strategy.h"

int cptSynchro;

 //! Retourne le premier bloc libre ou NULL si le cache est plein
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
	struct Cache_Block_Header *libre = pcache->pfree;
	pcache->pfree->flags |= VALID;
	// On parcours tous les autres block restante pour trouver un bloc 'free'
	for(int i = 0; i < pcache->nblocks; i++) {
		if((pcache->headers[i].flags & VALID)==0) {
			pcache->pfree = &pcache->headers[i];
			break;
		}
	}
	
	return libre;
}

void checkSynchronisation(struct Cache *pcache){
	if(++cptSynchro == NSYNC)
		Cache_Sync(pcache);
}

//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){
	//creation de la struct
	struct Cache * cache = malloc(sizeof(struct Cache));
	cache->file = fic;
	cache->fp = fopen(fic, "r+");
	cache->nblocks = nblocks;
	cache->nderef = nderef;
	cache->blocksz = recordsz * nrecords;
	cache->nrecords = nrecords;
	cache->recordsz = recordsz;
	cache->pstrategy = Strategy_Create(cache);
	//cache->instrument = (struct Cache_Instrument *)malloc(sizeof(struct Cache_Instrument));
	Cache_Get_Instrument(cache);
	struct Cache_Block_Header * headers = malloc(sizeof(struct Cache_Block_Header) * nblocks);
	cache->headers = headers;
	cache->pfree = Get_Free_Block(cache);
	cptSynchro = 0;

	return cache;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	//synchronise cache / fichier
	if(Cache_Sync(pcache) == CACHE_KO) return CACHE_KO;
	//close le fichier
	if(fclose(pcache->fp) == CACHE_KO) return CACHE_KO;
	//desalloue 
	Strategy_Close(pcache);
	free(&pcache->instrument);
	free(pcache->headers);
	free(pcache);
	return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
	//+1 au cpt de synchronisation
	pcache->instrument.n_syncs++;
	for(int i = 0 ; i < pcache->nblocks ; i++){
		//on regarde si il a été modifié
		if((pcache->headers[i].flags & MODIF) > 0){
			if(fseek(pcache->fp, i * pcache->blocksz, SEEK_SET) != 0) return CACHE_KO;
			if(fputs(pcache->headers[i].data, pcache->fp) == EOF) return CACHE_KO;
			//on remet le bit a modification a 0
			pcache->headers[i].flags &= ~MODIF;
		} 
	}
	return CACHE_OK;
}


//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
	for(int i = 0 ; i < pcache->nblocks ; i++)
		pcache->headers[i].flags &= ~VALID;

	Strategy_Invalidate(pcache);
	return CACHE_OK;
}

struct Cache_Block_Header * getBlockByIbfile(struct Cache *pcache, int irfile){
	struct Cache_Block_Header * header;
	for(int i = 0 ; i < pcache->nblocks ; ++i){
		header = &pcache->headers[i];
		if(header->ibfile == irfile){
			return &header[i];
		}
	}
	return NULL;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){
	struct Cache_Block_Header * header;
	if((header = getBlockByIbfile(pcache, irfile)) == NULL){//si le block n'est pas dans le cache
		header = Strategy_Replace_Block(pcache);
		if(fseek(pcache->fp, header->ibfile * pcache->blocksz, SEEK_SET) != 0) return CACHE_KO;
		if(fputs((char *)pcache->fp, (FILE *)header->data) == EOF) return CACHE_KO;	
		//+1 au nombre d'enregistrement dans le cache
		pcache->instrument.n_hits++;
	}
	//on copie dans le buffer
	if(fputs(header->data, (FILE *)precord) == EOF) return CACHE_KO;
	//+1 au nombre de lecture
	pcache->instrument.n_reads++;
	checkSynchronisation(pcache);
	Strategy_Read(pcache, header);
	return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){
	struct Cache_Block_Header * header = getBlockByIbfile(pcache, irfile);
	if(header->flags & VALID == 0){//si le block n'est pas valide on change
		header = Strategy_Replace_Block(pcache);
		if(fseek(pcache->fp, header->ibfile * pcache->blocksz, SEEK_SET) != 0) return CACHE_KO;
		if(fputs((char *)pcache->fp, (FILE*)header->data) == EOF) return CACHE_KO;	
		//+1 au nombre d'enregistrement dans le cache
		pcache->instrument.n_hits++;
		header->flags |= MODIF;
	}
	if(fputs((char *)precord, (FILE *)header->data) == EOF) return CACHE_KO;
	//+1 au nombre d'écriture
	pcache->instrument.n_reads++;
	checkSynchronisation(pcache);
	Strategy_Write(pcache, header);
	return CACHE_OK;
}

struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache)
{
	struct Cache_Instrument stat = pcache->instrument;
	pcache->instrument.n_reads = 0;
	pcache->instrument.n_writes = 0;
	pcache->instrument.n_hits = 0;
	pcache->instrument.n_syncs = 0;
	pcache->instrument.n_deref = 0;
	return &stat;
}
