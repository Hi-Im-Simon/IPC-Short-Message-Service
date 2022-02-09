#ifndef LIST
#define LIST
#include "servers_config.h"
#include <time.h>

typedef struct user_channel_message{
    char message[MAX_MESSAGE_LENGTH+1];
    char sender[MAX_USERNAME_LENGTH+1];
    time_t date;
} user_channel_message;

struct last_messages_list_element;

typedef struct last_messages_list_element{
    user_channel_message data;
    struct last_messages_list_element* prev;
    struct last_messages_list_element* next;
} last_messages_list_element;

// pomocnicza struktura listy do przechowywania N ostatnich wiadomości
typedef struct last_messages_list{
    last_messages_list_element* front;
    last_messages_list_element* back;
    int length;
    int limit;
} last_messages_list;

// stworzenie listy, inicjalizacja wskaźników
void initLastMessagesList(last_messages_list* lml, int limit);

// wyczyszczenie pamięci
void clearLastMessagesList(last_messages_list* lml);

// dodaje nowy element do listy
void addElementToLastMessagesList(last_messages_list* lml, user_channel_message* data);

#endif