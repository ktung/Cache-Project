/*!
 * \file low_cache.h
 *
 * \brief Structures de données pour l'implémentation du cache.
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: low_cache.h,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#ifndef _LOW__CACHE_H_
#define _LOW_CACHE_H_

#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

/*!
 * \defgroup low_cache_interface Interface de réalisation interne du cache
 *
 * Ce module contient les structure de données cosntitutant la réalisation
 * interne du cache..
 */

//! Flags pour chaque bloc du cache.
/*!
 * \ingroup low_cache_interface
 *
 * Ces flags sont ceux qui ne dépendent pas de la stratégie de
 * remplacement. D'autres flags peuvent être ajoutés par les stratégies de
 * remplacement.
 */
typedef enum  {
    VALID = 0x1, //!< le bloc est valide
    MODIF = 0x2, //!< le bloc a été modifié
} Cache_Flag;

//! Entête de chaque bloc.
/*!
 * \ingroup low_cache_interface
 *
 * Cet entête, les informations permettant d'établir la correspondance entre
 * les blocs du ficheirs et ceux du cache, ainsi ,qu'un pointeur sur les
 * données proprement dites.
 */
struct Cache_Block_Header
{
    Cache_Flag flags; 	        //!< Indicateurs d'état.
    int ibfile;			//!< Index de ce block dans le fichier.
    int ibcache;		//!< Index de ce block dans le cache.
    char *data; 		//!< Les données de l'utilisateur.
};


/*! Le cache lui-même.
 *
 * \ingroup low_cache_interface
 *
 * Cette structure contient les paramètres de configuration (nom et flux du
 * fichier) ainsi que les paramètres de dimensionnement. La stratégie courante
 * n'est connue qu'à travers un pointeur \b opaque (\c pstrategy). 
 * 
 * On trouve aussi des données dynamiques de gestion comme celles liées à
 * l'instrumentation, le début de la liste libre (\c pfree) ainsi qu'un
 * pointeurs sur le tableau des blocs du cache (\c pheaders).
 *
 * \note Nous avons fait un petit coup de canif dans la moduularité (et
 * l'opacité) de la stratégie) en prévoyant ici un champ spécifique à une
 * stratégie donnée (NUR), la période de déréférençage (\c nderef). Il s'agit
 * en effet d'un paramètre de configuration du cache, passé lors de la création
 * de ce dernier. mais c'est un artefact du simulateur. Dans un cache réel,
 * cette période serait définie temporellement et non par un nombre d'accès.
 */
struct Cache
{
    char *file;		    	//!< Nom du fichier   
    FILE *fp;			//!< Pointeur sur fichier 
    unsigned int nblocks;	//!< Nb de blocs dans le cache
    unsigned int nrecords;	//!< Nombre d'enregistrements dans chaque bloc
    size_t recordsz;		//!< Taille d'un enregistrement
    size_t blocksz;		//!< Taille d'un bloc 
    unsigned int nderef;	//!< période de déréférençage pour NUR 
    void *pstrategy;		//!< Structure de données dépendant de la stratégie 
    struct Cache_Instrument instrument; //!< Instrumentation du cache 
    struct Cache_Block_Header *pfree;   //!< Premier bloc libre (invalide) 
    struct Cache_Block_Header *headers; //!< Les données elles-mêmes 
};

//! Fréquence de synchronisation
/*!
 * \ingroup low_cache_interface
 * 
 * En général un cache est synchroniseé à interval de temps régulier. ici, pour
 * ce simulateur, nous nous contentons d'une synchronisation tous les \c NSYNC
 * accès au cache.
 */
#define NSYNC 1000

//! Recherche d'un bloc libre.
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache);

//! Adresse de l'enregistrement d'indice-fichier \a ind dans le bloc pointé par \a pb
/*!
 * \ingroup low_cache_interface
 *
 * \param pcache pointeur sur le cache
 * \param ind indice de l'enregistrement dans le fichier
 * \param pb pointeur sur le bloc du cache
 * \return l'adresse en octet de l'enregistrement dans le cache
 */
#define ADDR(pcache, ind, pb) \
    ((pb)->data + ((ind) % (pcache)->nrecords) * (pcache)->recordsz)

//! Adresse (en octets) du bloc d'index-fichier \a ibfile dans le cache
/*!
 * \ingroup low_cache_interface
 *
 * \param pcache pointeur sur le cache
 * \param ibfile indice du bloc le fichier
 * \return l'adresse en octet de l'enregistrement dans le fichier
 */
#define DADDR(pcache, ibfile) ((ibfile) * (pcache)->blocksz) 

#endif /* _LOW_CACHE_H_ */
