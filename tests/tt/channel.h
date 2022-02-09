#ifndef CHANNEL_H
#define CHANNEL_H
#include "servers_config.h"
#include "list.h"
struct User;

// struktura kanału
typedef struct Channel{
    char name[MAX_CHANNEL_NAME_LENGTH+1];
    int usersLength;
    struct User* users[MAX_USERS_NUMBER];

    last_messages_list lastMessages;
} Channel;

// pomocznica funkcja do szukania po napisie odpowiedniego kanału
Channel* findChannel(char* channelName, Channel* channels, int channelsLength);

// sprawdza czy kanał posiada użytkownika
int hasUser(struct User* user, Channel* channel);

// sprawdza czy kanał posiada użytkownika
struct User* hasUserById(long userid, Channel* channel);
#endif