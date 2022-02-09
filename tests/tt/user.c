#include "user.h"
#include <string.h>

int isUsernameUnique(int usersLength, User users[usersLength], char* newUsername){
    int i;
    for(i=0; i<usersLength; i++){
        if(strcmp(users[i].username, newUsername) == 0) return -1;
    }
    return 0;
}

User* findUserByName(int usersLength, User users[usersLength], char* newUsername){
    int i;
    for(i=0; i<usersLength; i++){
        if(strcmp(users[i].username, newUsername) == 0) return &users[i];
    }
    return NULL;
}

User* findUser(long userId, User* users, int usersLength){
    int i;
    for(i=0; i<usersLength; i++){
        if(users[i].userId == userId){
            return &users[i];
        }
    }
    return NULL;
}

int hasChannel(User* user, Channel* channel){
    int i;
    for(i=0; i<user->channelsLength; i++){
        if(strcmp(channel->name, user->channels[i]->name) == 0) return 1;
    }
    return 0;
}

void removeChannel(User* user, Channel* channel){
    int i, j;
    for(i=0; i<user->channelsLength; i++){
        if(strcmp(channel->name, user->channels[i]->name) == 0){
            for(j=i; j<user->channelsLength-1; j++){
                user->channels[j] = user->channels[j+1];
            }
            user->channelsLength--;
            break;
        }
    }
}