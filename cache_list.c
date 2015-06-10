/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO
 * \author Aina Rapenomanana
 * \author Pascal Tung
 */

#include <stdlib.h>
#include "cache_list.h"

/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create()
{
    struct Cache_List* new_list = (struct Cache_List*)malloc(sizeof(struct Cache_List));
    new_list->pheader = NULL;
    new_list->prev    = new_list;
    new_list->next    = new_list;

    return new_list;
}

/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list)
{
    if (!Cache_List_Is_Empty(list))
    {
       for (struct Cache_List* tmp = list->next; tmp != list; tmp = tmp->next)
        {
            tmp->pheader    = NULL;
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            free(tmp);
        }
    }

    list->pheader = NULL;
    free(list);
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_List *new_elem = Cache_List_Create();
    new_elem->pheader = pbh;

    struct Cache_List *last = list->next;
    for (; last != list; last = last->next);

    new_elem->prev   = last->prev;
    last->prev->next = new_elem;
    last->prev       = new_elem;
    new_elem->next   = last;
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_List *new_elem = Cache_List_Create();
    new_elem->pheader = pbh;

    struct Cache_List *second = list->next;

    new_elem->prev     = second->prev;
    second->prev->next = new_elem;
    second->prev       = new_elem;
    new_elem->next     = second;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list)
{
    struct Cache_List* first = list->next;
                    
    first->prev->next = first->next;
    first->next->prev = first->prev;

    struct Cache_Block_Header *header = first->pheader;
    free(first);
    return header;
}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list)
{
    struct Cache_List* last;
    for (last = list->next; last != list; last = last->next);

    last->prev->next = last->next;
    last->next->prev = last->prev;

    struct Cache_Block_Header *header = last->pheader;
    free(last);
    return header;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    struct Cache_List* el = list;
    for (el = list->next; el->pheader != pbh; el = el->next);
            
    el->prev->next = el->next;  
    el->next->prev = el->prev;

    struct Cache_Block_Header *header = el->pheader;
    free(el);
    return header;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list){
    for (struct Cache_List* tmp = list->next; tmp != list; tmp = tmp->next)
    {
        tmp->pheader    = NULL;
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        free(tmp);
    }

    list->pheader = NULL;
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list){
    return list->next == list->prev;
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    Cache_List_Remove(list, pbh);
    Cache_List_Append(list, pbh);
}

/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    Cache_List_Remove(list, pbh);
    Cache_List_Prepend(list, pbh);
}
