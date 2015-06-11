
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

//compteur de lecture / ecriture
int cptSynchro;

//! Verifie si le cache doit etre synchronisé avec le fichier
void Check_Synchronisation(struct Cache *pcache){
    if(++cptSynchro == NSYNC)
        Cache_Sync(pcache);
}

//! initialise flag : rend le block valide / non modifié
void Init_Flags(struct Cache_Block_Header * header){
    header->flags |=VALID;
    header->flags &= ~MODIF;
}

 //! Retourne le premier bloc libre ou NULL si le cache est plein
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache)
{
    struct Cache_Block_Header *libre = NULL;
    // On parcours tous les autres block restante pour trouver un bloc 'free'
    for(int i = 0; i < pcache->nblocks; i++) {
        if((pcache->headers[i].flags & VALID)==0) {
            libre = &pcache->headers[i];
            Init_Flags(libre);
            break;
        }
    }
    
    return libre;
}

//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){
    //creation de la struct
    struct Cache * cache = malloc(sizeof(struct Cache));
    cache->file = fic;
    cache->fp = fopen(fic, "wb+");
    cache->nblocks = nblocks;
    cache->nderef = nderef;
    cache->blocksz = recordsz * nrecords;
    cache->nrecords = nrecords;
    cache->recordsz = recordsz;
    cache->pstrategy = Strategy_Create(cache);
    Cache_Get_Instrument(cache);
    struct Cache_Block_Header * headers = malloc(sizeof(struct Cache_Block_Header) * nblocks);
    cache->headers = headers;

    //initialisation des headers
    for(int i = 0 ; i < nblocks ; ++i){
        cache->headers[i].ibcache = i;
        cache->headers[i].flags = 0;
        cache->headers[i].data = malloc(nrecords * recordsz);
    }

    //initialisation du premier block free
    cache->pfree = &cache->headers[0];
    cptSynchro = 0;

    return cache;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
    //synchronise cache / fichier
    if(Cache_Sync(pcache) == CACHE_KO) return CACHE_KO;
    //close le fichier
    if(fclose(pcache->fp) != 0) return CACHE_KO;
    //Ferme la strategie
    Strategy_Close(pcache);

    //on free les datas
    for(int i = 0 ; i < pcache->nblocks ; ++i)
        free(pcache->headers[i].data);

    //free de la struct cache
    free(pcache->headers);
    free(pcache);

    return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
    //reinit cpt de synchronisation
    cptSynchro = 0;
    //+1 au nombre de synchronisation
    pcache->instrument.n_syncs++;

    for(int i = 0 ; i < pcache->nblocks ; i++){
        //on regarde si il a été modifié
        if((pcache->headers[i].flags & MODIF) > 0){
            if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET) != 0) return CACHE_KO;
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
    //reinit le block free
    pcache->pfree = &pcache->headers[0];
    //reinit les stats
    Cache_Get_Instrument(pcache);
    //applique a la strategie
    Strategy_Invalidate(pcache);

    return CACHE_OK;
}

//! retourne le block en fonction de l'indice d'enregistrement dans le fichier
struct Cache_Block_Header * getBlockByIbfile(struct Cache *pcache, int irfile){
    // Indice du bloc contenant l'enregistrement
    int ibSearch = irfile / pcache->nrecords; 
    for(int i = 0 ; i < pcache->nblocks ; ++i){
        if(pcache->headers[i].flags & VALID){//si le block est valide
            if(pcache->headers[i].ibfile == ibSearch)//si il contient les bonnes infos
                return &pcache->headers[i];
        }
    }

    return NULL;
}

//! retourne le block correspondant a l'irfile passé en param
struct Cache_Block_Header * Read_In_Cache(struct Cache *pcache, int irfile){
    struct Cache_Block_Header * header = getBlockByIbfile(pcache, irfile);
    //si le block n'est pas dans le cache
    if(header == NULL){
        header = Strategy_Replace_Block(pcache);
        header->ibfile = irfile / pcache->nrecords;
        if(fseek(pcache->fp, DADDR(pcache, header->ibfile), SEEK_SET) != 0) return CACHE_KO;
        if(fgets(header->data, pcache->blocksz, pcache->fp) == EOF) return CACHE_KO; 
        //MAJ des flags
        Init_Flags(header);
    } else 
    {
        //l'élément est dans le cache
        pcache->instrument.n_hits++;
    }

    return header;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){
    struct Cache_Block_Header * header = Read_In_Cache(pcache, irfile);
    //on copie dans le buffer
    memcpy(precord, ADDR(pcache, irfile, header) , pcache->recordsz);
    //+1 au nombre de lecture
    pcache->instrument.n_reads++;
    Check_Synchronisation(pcache);
    Strategy_Read(pcache, header);

    return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){
    struct Cache_Block_Header * header = Read_In_Cache(pcache, irfile);
    //on copie dans le bloc
    memcpy(ADDR(pcache, irfile, header), precord, pcache->recordsz);
    header->flags |= MODIF;
    //+1 au nombre d'écriture
    pcache->instrument.n_writes++;
    Check_Synchronisation(pcache);
    Strategy_Write(pcache, header);

    return CACHE_OK;
}

// retourne les stats et les reinit
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache)
{
    struct Cache_Instrument stat = pcache->instrument;
    pcache->instrument.n_reads   = 0;
    pcache->instrument.n_writes  = 0;
    pcache->instrument.n_hits    = 0;
    pcache->instrument.n_syncs   = 0;
    pcache->instrument.n_deref   = 0;
    
    return &stat;
}
