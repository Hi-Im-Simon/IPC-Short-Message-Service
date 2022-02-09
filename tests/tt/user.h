#ifndef USER_H
#define USER_H
#include "servers_config.h"
#include "channel.h"

// struktura użytkownika
// użytkownik póki co ma tylko swoje id i nazwę
typedef struct User{
    char username[MAX_USERNAME_LENGTH+1];
    long userId;
    Channel* channels[MAX_CHANNELS_NUMBER];
    int channelsLength;
    int isSignedIn;
    int heartbeatProcess;
} User;



// deklaracje metod pomoczniczych

// pomocnicza funkcja sprawdzająca czy @newUsername jest unikalny w tablicy users
int isUsernameUnique(int usersLength, User users[usersLength], char* newUsername);

// pomocznica funkcja do szukania po id odpowiedniego użytkownika
User* findUser(long userId, User* users, int usersLength);

// pomocnicza funkcja do szukania po nazwie użytkownika
User* findUserByName(int usersLength, User users[usersLength], char* newUsername);

// sprawdza czy użytkownik nie posiada już tego kanału
int hasChannel(User* user, Channel* channel);

// usuwa użytkownikowi kanał
void removeChannel(User* user, Channel* channel);

#endif