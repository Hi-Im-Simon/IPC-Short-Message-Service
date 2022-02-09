#include "list.h"
#include <stdlib.h>


void initLastMessagesList(last_messages_list* lml, int limit){
    lml->front = lml->back = NULL;
    lml->length = 0;
    lml->limit = limit;
}
void clearLastMessagesList(last_messages_list* lml){
    last_messages_list_element* lmle = lml->front;
    while(lmle!=lml->back){
        lmle = lmle->next;
        free(lmle->prev);
    }
    free(lml->back);
    lml->front = lml->back = NULL;
    lml->length = 0;
}

void addElementToLastMessagesList(last_messages_list* lml, user_channel_message* data){
    if(lml->limit <= 0) return;
    
    if(lml->length == lml->limit){ // usun pierwszego, przesun w lewo
        if(lml->front->next != NULL){ // usun pierwszego, drugi bedzie pierwszy
            lml->front = lml->front->next;
            free(lml->front->prev);
            lml->front->prev = NULL;
        }else{ // usun pierwszego i jedynego
            free(lml->front);
            lml->front = NULL;
            lml->back = NULL;
        }
    }else lml->length++;
    
    if(lml->front == NULL){ // dodaj pierwszy element
        lml->front = (last_messages_list_element*) malloc(sizeof(last_messages_list_element));
        lml->front->data = *data;
        lml->front->prev = NULL;
        lml->front->next = NULL;
        lml->back = lml->front;
    }else{ // dodaj na koniec
        lml->back->next = (last_messages_list_element*) malloc(sizeof(last_messages_list_element));
        lml->back->next->data = *data;
        lml->back->next->prev = lml->back;
        lml->back->next->next = NULL;
        lml->back = lml->back->next;
    }
}