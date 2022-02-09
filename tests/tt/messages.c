#include "messages.h"
#include <string.h>
#include <stdio.h>

int getClientEnterMessage(message* msg, char username[MAX_USERNAME_LENGTH+1]){
    if(strlen(username) <= MAX_USERNAME_LENGTH){
        msg->type = CLIENT_ENTER_MESSAGE_TYPE;
        strcpy(msg->data.cem.username, username);
        return 0;
    }
    return -1; 
}

void getClientLeaveMessage(message* msg, int userId){
    msg->type = CLIENT_LEAVE_MESSAGE_TYPE;
    msg->data.um.id = userId;
}

void getClientListOfUsersMessage(message* msg, int userId){
    msg->type = CLIENT_LIST_OF_USERS_MESSAGE_TYPE;
    msg->data.um.id = userId;
}

void getClientListOfChannelsMessage(message* msg, int userId){
    msg->type = CLIENT_LIST_OF_CHANNELS_MESSAGE_TYPE;
    msg->data.um.id = userId;
}

void getClientJoinChannelMessage(message* msg, int userId, char* channelName){
    msg->type = CLIENT_JOIN_CHANNEL_MESSAGE_TYPE;
    msg->data.ccm.userid = userId;
    strcpy(msg->data.ccm.channelName, channelName);
}

void printSJCM(int sjcmCode){
    switch(sjcmCode){
        case -3:
            printf("Serwer osiagnal juz limit kanalow. Dodanie nowego jest niemozliwe.\n");
        break;
        case -2:
            printf("Jestes juz zapisany do tego kanalu.\n");
        break;
        case -1:
            printf("Blad wewnetrzny. Sprobuj ponownie pozniej.\n"); // bledne id uzytkownika
        break;
        case 0:
            printf("Pomyslnie dodano do kanalu\n");
        break;
    }
}

void printSLCM(int slcmCode){
    switch(slcmCode){
        case -4:
            printf("Nie mozna opuscic tego kanalu.\n");
        break;
        case -3:
            printf("Nie nalezysz do tego kanalu.\n");
        break;
        case -2:
            printf("Taki kanal nie istnieje.\n");
        break;
        case -1:
            printf("Blad wewnetrzny. Sprobuj ponownie pozniej.\n"); // bledne id uzytkownika
        break;
        case 0:
            printf("Pomyslnie opuszczono kanal\n");
        break;
    }
}

void getClientListOfChannelsWithUsersMessage(message* msg, int userId){
    msg->type = CLIENT_LIST_OF_CHANNELS_WITH_USERS_MESSAGE_TYPE;
    msg->data.um.id = userId;
}

void getClientLeaveChannelMessage(message* msg, int userId, char* channelName){
    msg->type = CLIENT_LEAVE_CHANNEL_MESSAGE_TYPE;
    msg->data.ccm.userid = userId;
    strcpy(msg->data.ccm.channelName, channelName);
}

void getClientPublicChannelMessage(message* msg, int userId, char* channelName, time_t date, char* text){
    msg->type = CLIENT_PUBLIC_CHANNEL_MESSAGE_TYPE;
    msg->data.cpcm.userid = userId;
    msg->data.cpcm.date = date;
    strcpy(msg->data.cpcm.channelName, channelName);
    strcpy(msg->data.cpcm.message, text);
}

void getClientPrivateMessage(message* msg, int userId, char* receiver, time_t date, char* text){
    msg->type = CLIENT_PRIVATE_MESSAGE_TYPE;
    msg->data.cpm.userid = userId;
    msg->data.cpm.date = date;
    strcpy(msg->data.cpm.receiver, receiver);
    strcpy(msg->data.cpm.message, text);
}

int getPublicMessageTypeToHandleMessages(int userId){
    return userId*MAX_USERS_NUMBER+1;
}

int getPrivateMessageTypeToHandleMessages(int userId){
    return userId*MAX_USERS_NUMBER+2;
}

int getHeartbeatReceiveTypeToHandleMessages(int userId){
    return userId*MAX_USERS_NUMBER+3;
}

int getHeartbeatSendTypeToHandleMessages(int userId){
    return userId*MAX_USERS_NUMBER+4;
}

void getClientSendHeatbeatMessage(message* msg, int userid){
    msg->type = getHeartbeatSendTypeToHandleMessages(userid);
}


