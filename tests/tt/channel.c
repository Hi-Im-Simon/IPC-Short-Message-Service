#include "channel.h"
#include "user.h"
#include <string.h>

Channel* findChannel(char* channelName, Channel* channels, int channelsLength){
    int i;
    for(i=0; i<channelsLength; i++){
        if(strcmp(channelName, channels[i].name) == 0){
            return &channels[i];
        }
    }
    return NULL;
}

int hasUser(User* user, Channel* channel){
    int i;
    for(i=0; i<channel->usersLength; i++){
        if(user->userId == channel->users[i]->userId){
            return 1;
        }
    }
    return 0;
}

User* hasUserById(long userid, Channel* channel){
    int i;
    for(i=0; i<channel->usersLength; i++){
        if(userid == channel->users[i]->userId){
            return channel->users[i];
        }
    }
    return NULL;
}