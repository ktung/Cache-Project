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
	
	return *cache_list;
}
/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list)
{
	Cache_List *current = list;
	Cache_List *temp;
	
	if (current->next == NULL && current != NULL)
		free(current);
	while (current->next != NULL)
	{
		current = temp;
		current = temp->next;
		free(temp);
	}
}

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
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
	Cache_List *new = Cache_List_Create();
	
	new->pheader = pbh;
	new->next = list;
	list->prev = new;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list)
{
	Cache_Block_Header *datas = list->pheader;
	Cache_List temp = list;
	
	list = list->next;
	free(temp);
	
	return datas;
}
/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list)
{
	Cache_Block_Header *datas;
	Cache_List *temp = list, *beforeLast;
	
	while (temp->next != NULL)
	{
		beforeLast = temp;
		temp = temp->next;
	}
	datas = temp->pheader;
	list = temp;
	beforeLast->next = NULL;
	free(temp);
	
	return datas;
}
/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
	Cache_List *temp = list, *before;
	Cache_Block_Header *datas = NULL;
	
	while (temp->pheader != pbh && temp->next != NULL)
	{
		before = temp;
		temp = temp->next;
	}
	if (temp != NULL)
	{
		before->next = temp->next;
		temp->next->prev = before;
		datas = temp->pheader;
		free(temp);
	}
	
	return datas;
}
                              

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
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
	Cache_List *temp = list, *before;
	
	while (temp->pheader != pbh && temp->next != NULL)
	{
		before = temp;
		temp = temp->next;
	}
	if (temp != NULL)
	{
		before->next = temp->next;
		temp->next->prev = before;
		free(temp);
		Cache_List_Append(list, pbh);
	}
}
                            
/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
	Cache_List *temp = list, *before;
	
	while (temp->pheader != pbh && temp->next != NULL)
	{
		before = temp;
		temp = temp->next;
	}
	if (temp != NULL)
	{
		before->next = temp->next;
		temp->next->prev = before;
		free(temp);
		Cache_List_Prepend(list, pbh);
	}
}
