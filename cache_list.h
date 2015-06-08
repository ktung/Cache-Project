#ifndef _CACHE_LIST_
#define _CACHE_LIST_
/*!
 * \file 
 * 
 * \brief Une liste simplifiée des headers de bloc du cache
 *
 * \author Jean-Paul Rigault 
 *
 */

#include <stdbool.h>

/*! La cellule de liste doublement chainée */
struct Cache_List
{
    struct Cache_Block_Header *pheader;	/* information */
    struct Cache_List *next;		/* chainage avant */
    struct Cache_List *prev;		/* chainage arrière */

};
/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create();
/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list);

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh);
/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh);

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list);
/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list);
/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh);

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list);

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list);

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list,
                            struct Cache_Block_Header *pbh);
/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh);

#endif /* CACHE_LIST_ */
