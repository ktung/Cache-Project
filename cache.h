/*!
 * \file cache.h
 *
 * \brief Interface du cache.
 * 
 * \note Ce fichier (et donc l'utilisateur du cache), n'a pas besoin de
 * connaitre les détails de l'implémentation du cache (struct Cache) : c'est un
 * exemple de type \b opaque. 
 *
 * \author Jean-Paul Rigault 
 */

#ifndef _CACHE_H_
#define _CACHE_H_

/*!
 * \defgroup cache_interface Interface utilisateur du cache
 *
 * Ces fonctions constituent l'API du cache, c'est-à-dire les fonctions
 * externes utilisables par le code client.
 */

#include <stdlib.h>

//! Code d'erreur
/*!
 * \ingroup cache_interface
 */
typedef enum {
    CACHE_KO = 0, //!< Ça va pas
    CACHE_OK,     //!< Tout va bien
} Cache_Error;

//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef);

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache);

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache);

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache);

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord);

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord);

//! Instrumentation du cache.
/*!
 * \ingroup cache_interface
 */
struct Cache_Instrument
{
    unsigned n_reads; 	//!< Nombre de lectures.
    unsigned n_writes;	//!< Nombre d'écritures.
    unsigned n_hits;	//!< Nombre de fois où l'élément était déjà dans le cache.
    unsigned n_syncs;	//<! Nombre d'appels à Cache_Sync().
    unsigned n_deref;	//!< Nombre de déréférençage (stratégie NUR).
};

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache);

#endif /* _CACHE_H_ */
