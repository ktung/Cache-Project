#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "cache_list.h"


/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create()
{

    struct Cache_List* l = malloc(sizeof(struct Cache_List));
    l->pheader=NULL;
    l->prev=l;
    l->next=l;

    return l;
}


/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list)
{

    struct Cache_List* pcurr;

    for (pcurr = list->next; pcurr != list; pcurr = pcurr->next){ 

        pcurr->prev->next = pcurr->next;
        pcurr->next->prev = pcurr->prev;
        free(pcurr);
    }

    free(list);
}


/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List * new = malloc(sizeof(struct Cache_List)); 
    new->pheader=pbh; 
    struct Cache_List * debut = list->next; 
    for(debut; debut!=list; debut=debut->next)
    {
    }
    new->prev = debut->prev; 
    debut->prev->next = new; 
    debut->prev = new; 
    new->next = debut; 
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List * new = malloc(sizeof(struct Cache_List)); 
    new->pheader=pbh; 
    struct Cache_List * debut = list->next; 
    new->prev = debut->prev; 
    debut->prev->next = new; 
    debut->prev = new; 
    new->next = debut; 

}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){

    struct Cache_List* first = list->next;
                    
    first->prev->next=first->next;  
    first->next->prev=first->prev;

    return first->pheader;

}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){

            struct Cache_List* last;// = list->prev;
            for (last = list->next; last != list; last=last->next){
            }
                    
            last->prev->next=last->next;    
            last->next->prev=last->prev;

            return last->pheader;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh){

            struct Cache_List* element = list;
            for (element = list->next; element->pheader != pbh; element=element->next){
            }
                    
            element->prev->next=element->next;  
            element->next->prev=element->prev;

            return element->pheader;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list){

    struct Cache_List * pcurr;

    for (pcurr = list->next; pcurr != list; pcurr = pcurr->next){ 
        (pcurr->prev)->next = pcurr->next;
        (pcurr->next)->prev = pcurr->prev;
            free(pcurr);
    }
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list){

    if (list->next == list->prev){
        return true;
    }

    else return false;
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh){

    Cache_List_Remove(list, pbh); 
    Cache_List_Append(list, pbh); 
        
}

/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh){

    Cache_List_Remove(list, pbh); 
    Cache_List_Prepend(list, pbh); 

}