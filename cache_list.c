/*!
 * \file low_cache.c
 * 
 * \author Thibaut SORIANO
 * \author Aina Rapenomanana
 * \Contributor Pascal Tung
 */

#include <stdlib.h>
#include "cache_list.h"

 void print_debug(struct Cache_List *list)
 {
    for (struct Cache_List *tmp = list; tmp; tmp = tmp->next)
    {
        printf("prev : %d header(%d) : %d next %d\n", tmp->prev, tmp, tmp->pheader, tmp->next);
    }
 }

/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create()
{
    struct Cache_List *new_list = (struct Cache_List*)malloc(sizeof(struct Cache_List));
    
    new_list->pheader = NULL;
    new_list->next = NULL;
    new_list->prev = NULL;
    
    return new_list;
}

/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list)
{
    printf("DELETE 1\n");
    if (Cache_List_Is_Empty(list))
    {
        free(list);
        return;
    }

    for (struct Cache_List *tmp = list->next; tmp; tmp = tmp->next)
    {
        list->pheader = NULL;
        list->prev = NULL;
        list->next = NULL;
        free(list);
        list = tmp;
    }
    printf("DELETE 2\n");
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    printf("APPEND 1 pbh = %d %d %d\n", pbh, list->pheader, list->next);
    // print_debug(list);
    if (Cache_List_Is_Empty(list))
    {
        list->pheader = pbh;
    // printf("APPEND 2 pbh = %d %d %d\n", pbh, list->pheader, list->next);
    //     print_debug(list);

        return;
    }

    // On cherche le dernier élément de la liste
    struct Cache_List *last = list;
    for (; last->next; last = last->next);

    // printf("APPEND 1T pbh = %d %d %d\n", pbh, list->pheader, list->next);
    // On place le nouvel élément à la fin
    struct Cache_List *newList = Cache_List_Create();
    // printf("APPEND 1T2 pbh = %d %d %d\n", pbh, list->pheader, list->next);
    // printf("APPEND 1T2 pbh = %d %d %d\n", pbh, newList->pheader, newList->next);
    newList->pheader = pbh;
    newList->prev = last;
    last->next = newList;
    // printf("APPEND 3 pbh = %d %d %d\n", pbh, list->pheader, list->next);
    //     print_debug(list);

}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
        // printf("PREPEND 1 %d %d\n\n", list, list->next);

    if (Cache_List_Is_Empty(list))
    {
        list->pheader = pbh;
        return;
    }

    // On cherche le premier élément de la liste
    struct Cache_List *first = list;
    for (; first->prev; first = first->prev);

    // On place le nouvel élément au début
    struct Cache_List *newList = Cache_List_Create();
    
    newList->pheader = pbh;
    newList->next = first;
    first->prev = newList;
        // printf("PREPEND 2 %d %d\n\n", list, list->next);

}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list)
{
    // printf("REMOVE 1");
    // print_debug(list);
    if (Cache_List_Is_Empty(list)) return NULL;

    struct Cache_List *first = list;
    for (; first->prev; first = first->prev);

    struct Cache_List *second = first->next;

    second->prev = NULL;
    first = second;

    // free(first);



    // // On cherche le premier élément de la liste
    // struct Cache_List *first = list;
    // for (; first->prev; first = first->prev);
    // struct Cache_Block_Header *firstHeader = first->pheader;


    // if (first == list)
    // {
    //     list = first->next;
    //     list->prev = NULL;
    //     free(first);
    // return firstHeader;
    // }

    // if (first->next)
    // {
    //     first->next->prev = NULL;
    //     first->next = NULL;
    // }

    //     free(first);
    
    // printf("REMOVE 2 %d %d\n", list->prev, list->next);
    // print_debug(list);
    return list->pheader;
}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list)
{
    if (Cache_List_Is_Empty(list)) return NULL;

    // On cherche le dernier élément de la liste
    struct Cache_List *last = list;
    for (; last->prev; last = last->prev);

    if (last->prev)
        last->prev->next = NULL;

    struct Cache_Block_Header *lastHeader = last->pheader;
    free(last);

    return lastHeader;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    printf("REMOVEA 1 pbh = %d %d %d\n", pbh, list->pheader, list->next);
    if (Cache_List_Is_Empty(list)) return NULL;

    // Recherche de pbh dans la liste
    struct Cache_List *tmp = list;
    for (; tmp->next && tmp->pheader != pbh; tmp = tmp->next);

    // Si on a trouvé le bon bloc
    if (pbh == tmp->pheader)
    {
        tmp->pheader = NULL;
        if (tmp->next) // Si le bloc trouvé n'est pas le dernier
            tmp->next->prev = tmp->prev;
        if (tmp->prev) // Si le bloc trouvé n'est pas le premier
        {
            tmp->prev->next = tmp->next;
            free(tmp);
        }
    printf("REMOVEA 2 pbh = %d %d %d\n", pbh, list->pheader, list->next);
        return pbh;
    }

    return NULL;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list)
{
    printf("CLEAR\n");
    if (Cache_List_Is_Empty(list)) return;

    
    // Parcourir la liste jusqu'à trouver next == null (on arrive à la fin de la liste)
    // for(struct Cache_List *tmp = list; tmp->next; tmp = tmp->next) {
    //     tmp->pheader = NULL;
    //     if (tmp->prev)
    //         tmp->prev->next = NULL;
    // }

    // Traite le 1er élément
    // list->pheader = NULL;
    // list = list->next;
    // if (list->prev)
    //     list->prev->next = NULL;

    // // Traitement de tous les autres éléments
    // for (struct Cache_List *tmp = list; tmp; tmp = tmp->next)
    // {
    //     list->pheader = NULL;
    //     list->next = NULL;
    //     free(list);
    //     list = tmp;
    // }
    Cache_List_Delete(list);
    list = Cache_List_Create();
    printf("CLEAR 2\n");
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list)
{
    // printf("EMPTY 1 %d %d\n", list->next, list->pheader);
    return list->next && list->prev && !list->pheader;
}

/*!
 * Transférer un élément à la fin
 * Supprime le bloc de la liste, puis l'ajoute à la fin
 */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    Cache_List_Remove(list, pbh);
    Cache_List_Append(list, pbh);
}

/*!
 * Transférer un élément  au début
 * Supprime le bloc de la liste, puis l'ajoute au début
 */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh)
{
    Cache_List_Remove(list, pbh);
    Cache_List_Prepend(list, pbh);
}
