#ifndef _STRATEGY_H_
#define _STRATEGY_H_

/*!
 * \file strategy.h
 *
 * \brief Interface utilisée par les fonctions de gestion de la stratégie de remplacement.
 * 
 * \author Jean-Paul Rigault 
 *
 * $Id: strategy.h,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

struct Cache;
struct Cache_Block_Header;

/*!
 * \defgroup strategy_interface Interface de la stratégie de remplacement
 *
 * Ces fonctions sont utilisées par celles de l'API du cache pour gérer la
 * stratégie de remplacement. Elles sont définies dans les différents fichiers
 * de stratégie.
 *
 * @{
 */

//! Creation et initialisation de la stratégie (invoqué par la création de cache).
void *Strategy_Create(struct Cache *pcache);

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache);

//! Fonction "réflexe" lors de l'invalidation du cache.
void Strategy_Invalidate(struct Cache *pcache);

//! Algorithme de remplacement de bloc.
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache);

//! Fonction "réflexe" lors de la lecture.
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb);

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb);

//! Identification de la stratégie.
char *Strategy_Name();

/* 
 * @} 
 */

#endif /* _STRATEGY_H_ */
