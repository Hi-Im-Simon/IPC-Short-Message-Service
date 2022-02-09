#ifndef MESSAGES_H
#define MESSAGES_H
#include "servers_config.h"
#include <time.h>

// Wartości typów komunikacji klient-server. Później serwer jako typ podaje id użytkownika.

#define CLIENT_ENTER_MESSAGE_TYPE 1
#define SERVER_ENTER_MESSAGE_TYPE 999999
#define CLIENT_LEAVE_MESSAGE_TYPE 101
#define CLIENT_LIST_OF_USERS_MESSAGE_TYPE 111
#define CLIENT_LIST_OF_CHANNELS_MESSAGE_TYPE 112
#define CLIENT_JOIN_CHANNEL_MESSAGE_TYPE 113
#define CLIENT_LIST_OF_CHANNELS_WITH_USERS_MESSAGE_TYPE 114
#define CLIENT_LEAVE_CHANNEL_MESSAGE_TYPE 115
#define CLIENT_PUBLIC_CHANNEL_MESSAGE_TYPE 201
#define CLIENT_PRIVATE_MESSAGE_TYPE 202
#define SERVER_NO_HEARTBEAT_MESSAGE_TYPE 301

// pomocnicze struktury
typedef struct SimpleChannel{
    char name[MAX_CHANNEL_NAME_LENGTH+1];
    char users[MAX_USERNAME_LENGTH+1][MAX_USERS_NUMBER];
    int usersLength;
} SimpleChannel;

// struktury początkowej komunikacji

// wiadomość wysyłana podczas dołączania na serwer przez klienta
typedef struct client_enter_message{
    char username[MAX_USERNAME_LENGTH+1];
} client_enter_message; 

// odpowiedź serwera podczas dołączania klienta na serwer
typedef struct server_enter_message{
    char username[MAX_USERNAME_LENGTH+1];
    char channelName[MAX_CHANNEL_NAME_LENGTH+1];
    long id; // id uzytkownika lub -1 w razie niepowodzenia
} server_enter_message; 




// kolejne struktury do komunikacji

typedef struct user_message{
    long id;
} user_message;

typedef struct server_list_begin_message{
    int size;
} server_list_begin_message;

typedef struct server_list_element_of_users_message{
    char username[MAX_USERNAME_LENGTH+1];
    int online;
} server_list_element_of_users_message;

typedef struct server_list_element_of_channels_message{
    char channel[MAX_CHANNEL_NAME_LENGTH+1];
} server_list_element_of_channels_message;

// join or leave
typedef struct client_channel_message{
    char channelName[MAX_CHANNEL_NAME_LENGTH+1];
    long userid;
} client_channel_message;

typedef struct server_message{
    int code;
} server_message;

typedef struct server_list_element_of_channels_with_users_message{
    SimpleChannel channel;
} server_list_element_of_channels_with_users_message;

// msg od klienta do serwera z zawartością wiadomości, która ma zostać publicznie podana na serwer
typedef struct client_public_channel_message{
    char message[MAX_MESSAGE_LENGTH+1];
    long userid;
    char channelName[MAX_CHANNEL_NAME_LENGTH+1];
    time_t date;
} client_public_channel_message;

typedef struct client_private_message{
    char message[MAX_MESSAGE_LENGTH+1];
    long userid;
    char receiver[MAX_USERNAME_LENGTH+1];
    time_t date;
} client_private_message;

typedef struct server_public_channel_message{
    char message[MAX_MESSAGE_LENGTH+1];
    char username[MAX_USERNAME_LENGTH+1];
    char channelName[MAX_CHANNEL_NAME_LENGTH+1];
    time_t date;
} server_public_channel_message;

typedef struct server_private_message{
    char message[MAX_MESSAGE_LENGTH+1];
    char sender[MAX_USERNAME_LENGTH+1];
    char receiver[MAX_USERNAME_LENGTH+1];
    time_t date;
} server_private_message;

// unia i struktura komunikacji, która jest naprawdę wysyłana za każdym razem

// unia - przyjmuje zawsze tylko jeden z typów w niej zawartych
union message_data{
    client_enter_message cem;
    server_enter_message sem;
    user_message um;
    server_list_begin_message slbm;
    server_list_element_of_users_message sleoum;
    server_list_element_of_channels_message sleocm;
    client_channel_message ccm;
    server_message sm;
    server_list_element_of_channels_with_users_message sleocwum;
    client_public_channel_message cpcm;
    server_public_channel_message spcm;
    client_private_message cpm;
    server_private_message spm;
};

// struktura, która jest ciałem każdego komunikatu
typedef struct message{
    long type;
    union message_data data;
} message;




// deklaracje funkcji pomocniczych

// pomocnicza funkcja do generowania struktury message dla klienta, który chce się zarejestrować na serwer
// zwraca -1, gdy przekroczono długość nazwy użytkownika
int getClientEnterMessage(message* msg, char username[MAX_USERNAME_LENGTH+1]);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce się wylogować z serwera
void getClientLeaveMessage(message* msg, int userId);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce poznać wszystkich użytkowników na serwerze
void getClientListOfUsersMessage(message* msg, int userId);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce poznać wszystkie kanały na serwerze
void getClientListOfChannelsMessage(message* msg, int userId);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce dołączyć do kanału
void getClientJoinChannelMessage(message* msg, int userId, char* channelName);

// wypisuje teskt na ekranie o znaczeniu błędu (server join channel message)
void printSJCM(int sjcmCode);

// wypisuje teskt na ekranie o znaczeniu błędu (server leave channel message)
void printSLCM(int slcmCode);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce poznać wszystkie kanały i ich użytkowników na serwerze
void getClientListOfChannelsWithUsersMessage(message* msg, int userId);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce dołączyć do kanału
void getClientLeaveChannelMessage(message* msg, int userId, char* channelName);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce wysłac wiadomość na kanale
void getClientPublicChannelMessage(message* msg, int userId, char* channelName, time_t date, char* text);

// pomocnicza funkcja do generowania struktury message dla klienta, który chce wysłać prywatną wiadomość
void getClientPrivateMessage(message* msg, int userid, char* receiver, time_t date, char* text);

// zwraca identyfikator wiadomości kolejki (msgtyp) dla wiadomości na kanały
int getPublicMessageTypeToHandleMessages(int userId);

// zwraca identyfikator wiadomości kolejki (msgtyp) dla wiadomości prywatnej
int getPrivateMessageTypeToHandleMessages(int userId);

// zwraca identyfikator wiadomości kolejki (msgtyp) dla wiadomości heartbeat
// na tym msgtyp client odbiera informacje, że musi dać sygnał o życiu
int getHeartbeatReceiveTypeToHandleMessages(int userId);

// zwraca identyfikator wiadomości kolejki (msgtyp) dla wiadomości heartbeat
// na tym msgtyp client wysyła do serwera, że żyje
int getHeartbeatSendTypeToHandleMessages(int userId);

// pomocnicza funkcja do generowania struktury message dla kleinta, który chce wysłać sygnał, że jest aktywny 
void getClientSendHeatbeatMessage(message* msg, int userid);
#endif