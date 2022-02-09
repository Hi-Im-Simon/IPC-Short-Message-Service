#ifndef SERVERS_CONFIG_H
#define SERVERS_CONFIG_H

// stała dla ułatwienia
#define IPC_FLAGS 0777|IPC_CREAT

// maksymalna długosć nazwy użytkownika bez uwzględniania znaku końca napisu '\0'
#define MAX_USERNAME_LENGTH 100 
// ilość użytkowników, którą może obsłużyć jeden serwer
#define MAX_USERS_NUMBER 50
// maksymalna ilość kanałów
#define MAX_CHANNELS_NUMBER 100
// maksymalna długość nazwy kanału
#define MAX_CHANNEL_NAME_LENGTH 100
// maksymalna długość jednej wiadomości
#define MAX_MESSAGE_LENGTH 1000 
// ilość pokazywanych ostatnich wiadomości na kanale
#define LAST_MESSAGES_LENGTH 10
// czas po którym użytkownik dostanie informacje o nieaktywności
#define HEARTBEAT_SECONDS 300
// czas w jakim użytkownik musi wykonać jakąkolwiek aktywność (po otrzymaniu informacji o braku aktywności), aby nie zostać wylogowanym
#define HEARTBEAT_WAIT_SECONDS 60
#endif