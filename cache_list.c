/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO Aina Rapenomanana
 */

#include "cache_list.h"

/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create()
{
	struct Cache_List cache_list = malloc(sizeof(struct Cache_List));
	
	cache_list->pheader = NULL;
	cache_list->next = NULL;
	cache_list->prev = NULL;
}
/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list);

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	// pbh -> créer une Cache list
	struct Cache_List cache_list = Cache_List_Create();
	cache_list->pheader = pbh;

	// Parcourir la list jusqu'a trouver next == null
	struct Cache_List *tmp = list;
	for(; tmp->next != NULL; tmp=tmp->next);

	// Avec le dernier bloc, le next du dernier sera le nouveau
	tmp->next = cache_list;
	// le prev du nouveau sera l'ancien dernier
	cache_list->prev = tmp;

}
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
void Cache_List_Clear(struct Cache_List *list) {
	struct Cache_List *tmp = list;
	// Parcourir la list jusqu'a trouver next == null (on arrive à la fin de la liste)
	for(; tmp->next != NULL; tmp=tmp->next) {
		tmp->pheader = NULL;
	}

}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list) {
	bool res = true;
	for(; tmp->next != NULL; tmp=tmp->next) {
		if(tmp->pheader == NULL) res &= true;
	}
	return res;
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list,
                            struct Cache_Block_Header *pbh);
/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh);
