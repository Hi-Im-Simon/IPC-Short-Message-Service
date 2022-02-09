// Aplikacja klienta

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "messages.h"
#include "string_helper.h"
#include "servers_config.h"

// deklaracje

// funkcja pytająca użytkownika o nazwę użytkownika
// argument @max określa maksymalną długość nazwy
// czyta znaki dopóki niewystąpi znak nowej linii (inne białe znaki są dozwolone)
void getUsername(char* username, int max);

// funkcja do połączenia z jakimkolwiek serwerem
// zwraca 0 w razie z sukcesu i ujemną wartość gdy się nie udało
// -1 oznacza błąd funkcji getClientEnterMessage z pliku "messages.h" (tzn. przekroczenie długości nazwy użytkownika)
// -2 oznacza błąd podczas tworzenie kolejki IPC
// -3 oznacza błąd podczas wysyłania komunikatu
// -4 oznacza, że mamy limit użytkowników
// -5 oznacza, że nazwa nie jest unikalna
int connectToServer(int serverId, char* username, int* userId, char channels[MAX_CHANNEL_NAME_LENGTH+1][MAX_CHANNELS_NUMBER], int* channelsLength);

// funkcja do odłączenia się od aktualnego serwera
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int leaveServer(int id, int userId);

// funkcja wyświetlające listę użytkowników na serwerze
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int showUsersList(int id, int userId);

// funkcja wyświetlające listę kanałów na serwerze
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int showChannelsList(int id, int userId);

// funkcja dodająca użytkownika na kanał (jeżeli kanał nie istnieje to jest tworzony)
// zwraca 0 w razie sukcesu i ujemne wartości w razie problemów
// rezultat można przetłumaczyć funkcją @printSJCM z pliku "messages.h"
int joinChannel(int id, int userId, char* channelName);

// funkcja usuwająca użytkownika z kanału (kanał nigdy nie jest usuwany, nawet jeżeli jest pusty)
// zwraca 0 w razie sukcesu i ujemne wartości w razie problemów
int leaveChannel(int id, int userId, char* channelName);

// funkcja wyświetlające listę kanałów i ich użytkowników na serwerze
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int showChannelsWithUsersList(int id, int userId);

// funkcja wysyłająca wiadomość na kanał
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int sendMessageToChannel(int id, int userId, char* channelName, char* message);

// funkcja obsługująca wiadomości tekstowe
// działa jako osobny proces i wyświetla wiadomości
void handleMessages(int id, int userId);

// funkcja kończy wszystkie procesy
void finish();

// wyświetla wiadomość
void printMessage(server_public_channel_message* spcm);

// wyświetla wiadomość prywatną
void printPrivateMessage(server_private_message* spm);

// funkcja do wysyłania prywatnej wiadomości
// zwraca 0 w razie z sukcesu i -1 gdy się nie udało (błąd podczas wysyłania komunikatu)
int sendPrivateMessage(int id, int userId, char* receiver, char* mess);

// funkcja która odbiera od serwera komunikaty o braku sygnałów życia
void handleHeartbeat(int id, int userId);

// funkcja która wysyła do serwera informacje o aktywności
void sendHeartbeat(int id, int userId);

// funkcja, która odbiera od serwera kanały do których nalezy użytkownik
// przydatne dopiero, gdy użytkownik zaloguje się kolejny raz
void getChannels(int id, int userId, char channels[MAX_CHANNEL_NAME_LENGTH+1][MAX_CHANNELS_NUMBER], int* channelsLength);

// funkcja główna, zarządzająca
int main(int argc, char *argv[]) {
    int serverId;
    if(argc != 2){
        printf("Musisz podac id serwera jako pierwszy argument!\n");
        exit(-1);
    }else{
        serverId = atoi(argv[1]);
    }

    // łączenie z serwerem i podawanie nazwy użytkownika
    char username[MAX_USERNAME_LENGTH+1];
    getUsername(username, MAX_USERNAME_LENGTH);
    char channels[MAX_CHANNEL_NAME_LENGTH+1][MAX_CHANNELS_NUMBER];
    int channel = 0, channelsLength = 0;
    int userId;
    int ipcId = connectToServer(serverId, username, &userId, channels, &channelsLength);
    if(ipcId >= 0){
        getChannels(ipcId, userId, channels, &channelsLength);
        printf("\033[1;35mWitaj %s!\n\033[0;37m", username);
    }else{
        switch(ipcId){
            case -1:
                printf("Podano za dluga nazwe uzytkownika\n");
            break;
            case -2:
                printf("Nie udalo sie utworzyc kolejki IPC\n");
            break;
            case -3:
                printf("Nie udalo sie wyslac komunikatu!\n");
            break;
            case -4:
                printf("Niestety nie da sie utworzyc juz nowego konta - osiagnieto limit uzytkownikow\n");
            break;
            case -5:
                printf("Uzytkownik o tej nazwie jest juz zalogowany na serwerze\n");
            break;
        }
        return -1;
    }

    signal(SIGUSR1, finish);
    if(fork() == 0){
        handleMessages(ipcId, userId);
        exit(0);
    }

    if(fork() == 0){
        handleHeartbeat(ipcId, userId);
        exit(0);
    }

    // obsługa komend
    char* cmd;
    char** words;
    int i;
    while(1){
        printf("(%s): ", channels[channel]);

        char* cmd = readLine();
        sendHeartbeat(ipcId, userId);
        printf("\033[0;31m");
        if(strlen(cmd) >= 1){
            if(cmd[0] != '/'){
                if(channel == -1){
                    printf("Aby wysylac wiadomosci musisz najpierw dolaczyc do dowolnego kanalu!\n");
                }else{
                    sendMessageToChannel(ipcId, userId, channels[channel], cmd);
                }
            }else{
                words = split(cmd, ' ');
                if(strcmp(words[0], "/wyloguj") == 0){
                    if(leaveServer(ipcId, userId) < 0) printf("Nie udalo sie opuscic serwera\n");
                    else{
                        printf("Zegnaj!\n");
                        break;
                    }
                }else if(strcmp(words[0], "/lista") == 0){
                    if(strcmp(words[1], "uzytkownikow") == 0){
                        showUsersList(ipcId, userId);
                    }else if(strcmp(words[1], "kanalow") == 0){
                        showChannelsList(ipcId, userId);
                    }else if(strcmp(words[1], "kanalow+") == 0){
                        showChannelsWithUsersList(ipcId, userId);
                    }else{
                        if(words[1][0]!=EOF && strlen(words[1]) > 0){
                            printf("Podano bledny drugi argument. Dostepne listy do zobaczenia to [uzytkownikow/kanalow/kanalow+]\n");
                        }else{
                            printf("Podaj jako drugi argument jaka liste chcesz zobaczyc [uzytkownikow/kanalow/kanalow+]\n");
                        }
                    }
                }else if(strcmp(words[0], "/dolacz") == 0){
                    if(words[1][0]!=EOF && strlen(words[1]) > 0){
                        int r =joinChannel(ipcId, userId, words[1]);
                        if(r == 0){
                            strcpy(channels[channelsLength], words[1]);
                            channel = channelsLength++; // ustawia aktywny
                        }
                        printSJCM(r);
                    }else{
                        printf("Podaj jako drugi argument nazwe kanalu, na ktory chcesz dolaczyc\n");
                    }
                }else if(strcmp(words[0], "/wyjdz") == 0){
                    if(words[1][0]!=EOF && strlen(words[1]) > 0){
                        int r =leaveChannel(ipcId, userId, words[1]);
                        if(r == 0){
                            if(strcmp(channels[channel], words[1]) == 0){
                                channel = -0;
                            }

                            for(i=0; i<channelsLength; i++){
                                if(strcmp(channels[i], words[1]) == 0){    
                                    int j;
                                    for(j=i; j<channelsLength-1; j++){
                                        strcpy(channels[j], channels[j+1]);
                                    }
                                    break;
                                }
                            }
                            channelsLength--;
                        }

                        printSLCM(r);
                    }else{
                        printf("Podaj jako drugi argument nazwe kanalu, z ktorego chcesz wyjsc\n");
                    }
                }else if(strcmp(words[0], "/wybierz") == 0){
                    if(words[1][0]!=EOF && strlen(words[1]) > 0){
                        for(i=0; i<channelsLength; i++){
                            if(strcmp(channels[i], words[1]) == 0){
                                channel = i;
                                break;
                            }
                        }
                        if(i == channelsLength) printf("Najpierw musisz dolaczyc na ten kanal!\n");
                    }else{
                        printf("Podaj jako drugi argument nazwe kanalu, na ktorym chcesz pisac\n");
                    }
                }else if(strcmp(words[0], "/wyslij") == 0){
                    if(words[1][0]!=EOF && strlen(words[1]) > 0){
                        if(words[2][0]!=EOF){
                            sendPrivateMessage(ipcId, userId, words[1], &cmd[9+strlen(words[1])]);
                        }else{
                            printf("Podaj jako trzeci argument wiadomosc\n");
                        }
                    }else{
                        printf("Podaj jako drugi argument nazwe uzytkownika, do ktorego chcesz wyslac wiadomosc prywatna\n");
                    }
                }else if(strcmp(words[0], "/pomoc") == 0){
                    printf("Kazda komenda rozpoczyna sie od znaku \"/\".\n/wyloguj  -> wylogowywuje uzytkownika\n/lista uzytkownikow  -> wyswietla liste zarejestrowanych uzytkownikow\n/lista kanalow  -> wyswietla liste stworzonych kanalow\n/lista kanalow+  -> wyswietla liste kanalow i uzytkownikow, ktore do nich naleza\n/dolacz NAZWA_KANALU  -> dolaczenie do kanalu\n/wyjdz NAZWA_KANALU  -> wyjscie z kanalu\n/wybierz NAZWA_KANALU  -> ustaw dany kanal jako aktywny (np. aby wyslac na nim wiadomosc)\n/wyslij UZYTKOWNIK NAZWA_KANALU  -> wysyla wiadomosc prywatna do uzytkownika\n/zyje  -> komenda specjalna, ktora pozwala na przekazanie informacji o zyciu serwerowi\n");
                }else if(strcmp(words[0], "/zyje") != 0){
                    printf("Nieznana komenda! Wpisz /pomoc aby uzyskac liste dostepnych komend.\n");
                }

                i = 0;
                while(words[i][0]!=EOF){
                    free(words[i++]);
                }
                free(words[i]);
                free(words);
            }

        }
        printf("\033[0;37m");
        free(cmd);
    }
    kill(0, SIGUSR1);
    return 0;
}



// definicje

void getUsername(char* username, int max){
    char* helper = (char*) malloc((max+3)*sizeof(char));
    printf("\033[0;31m");
    do{
        printf("Podaj nazwe uzytkownika (maksymalna dlugosc to %d): \033[0;37m", max);
        fgets(helper, max+3, stdin);
        printf("\033[0;31m");
        if(strlen(helper) >= max+2){
            printf("Podales za dluga nazwe uzytkownika! Sprobuj jeszcze raz.\n");
            cleanBuffer();
        }else if(strlen(helper) == 1){
            printf("Nazwa uzytkownika musi zawierac co najmniej jeden znak!\n");
        }else break;
    }while(1);
    printf("\033[0;37m");
    helper[strlen(helper)-1] = '\0'; //remove '\n' character
    strcpy(username, helper);
    free(helper);
}

int connectToServer(int serverId, char* username, int* userId, char channels[MAX_CHANNEL_NAME_LENGTH+1][MAX_CHANNELS_NUMBER], int* channelsLength){
    message msg;
    if(getClientEnterMessage(&msg, username) == 0){
        int id = msgget(serverId, IPC_FLAGS);
        if(id != -1){
            if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
                while(1){
                    msgrcv(id, &msg, sizeof(msg.data), SERVER_ENTER_MESSAGE_TYPE, 0);
                    server_enter_message* sem = &msg.data.sem;
                    if(strcmp(sem->username, username) == 0){
                        if(sem->id >= 0){
                            *userId = sem->id;
                            strcpy(channels[0], sem->channelName);
                            (*channelsLength) = 1;
                            return id;
                        }else{
                            if(sem->id == -1) return -4;
                            else return -5;
                        }
                    }
                }
            }else return -3;
        }else return -2;
    }else return -1;
}

int leaveServer(int id, int userId){
    message msg;
    getClientLeaveMessage(&msg, userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        return 0;
    }else return -1;
}

int showUsersList(int id, int userId){
    message msg;
    getClientListOfUsersMessage(&msg, userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        int n = msg.data.slbm.size;
        int i;
        int condition;
        printf("\033[1;34mLista uzytkownikow:\n");
        for(i=0; i<n; i++){
            msgrcv(id, &msg, sizeof(msg.data), userId, 0);
            condition = msg.data.sleoum.online == 1;
            printf("- %s (\033[%dm%s\033[34m)\n", msg.data.sleoum.username, condition ? 32 : 31, condition ? "online" : "offline");
        }
        printf("\033[0;37m");
        return 0;
    }else return -1;
}

int showChannelsList(int id, int userId){
    message msg;
    getClientListOfChannelsMessage(&msg, userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        int n = msg.data.slbm.size;
        int i;
        printf("\033[1;34mLista kanalow:\n");
        for(i=0; i<n; i++){
            msgrcv(id, &msg, sizeof(msg.data), userId, 0);
            printf("- %s\n", msg.data.sleocm.channel);
        }
        printf("\033[0;37m");
        return 0;
    }else return -1;
}

int joinChannel(int id, int userId, char* channelName){
    message msg;
    getClientJoinChannelMessage(&msg, userId, channelName);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        return msg.data.sm.code;
    }else return -1;
}

int showChannelsWithUsersList(int id, int userId){
    message msg;
    getClientListOfChannelsWithUsersMessage(&msg, userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        int n = msg.data.slbm.size;
        int i, j;
        SimpleChannel* channel;
        printf("\033[1;34mLista kanalow:\n");
        for(i=0; i<n; i++){
            msgrcv(id, &msg, sizeof(msg.data), userId, 0);
            channel = &msg.data.sleocwum.channel;
            printf("- %s (%d)\n", channel->name, channel->usersLength);
            for(j=0; j<channel->usersLength; j++){
                printf("--- %s\n", channel->users[j]);
            }
        }
        printf("\033[0;37m");
        return 0;
    }else return -1;
}

int leaveChannel(int id, int userId, char* channelName){
    message msg;
    getClientLeaveChannelMessage(&msg, userId, channelName);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == 0){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        return msg.data.sm.code;
    }else return -1;
}

int sendMessageToChannel(int id, int userId, char* channelName, char* mess){
    int len = strlen(mess);
    int n = (len-1)/MAX_MESSAGE_LENGTH + 1;
    int i;
    message msg;
    char* helper;
    int ret = 0;
    for(i=0; i<n; i++){
        helper = substring(mess, i*len, (i+1)*len);
        getClientPublicChannelMessage(&msg, userId, channelName, time(NULL), helper);
        if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0) ret = -1;
        free(helper);
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        printMessage(&msg.data.spcm);
    }
    return ret;
}

void handleMessages(int id, int userId){
    message msg;
    if(fork() == 0){ // kanałowe
        int cmsgtyp = getPublicMessageTypeToHandleMessages(userId);
        while(1){
            if(msgrcv(id, &msg, sizeof(msg.data), cmsgtyp, 0) == -1) break;
            printMessage(&msg.data.spcm);
        }
    }else{ // prywatne
        int pmsgtyp = getPrivateMessageTypeToHandleMessages(userId);
        while(1){
            if(msgrcv(id, &msg, sizeof(msg.data), pmsgtyp, 0) == -1) break;
            printPrivateMessage(&msg.data.spm);
        }
    }
    
}

void finish(){
    exit(0);
}

void printMessage(server_public_channel_message* spcm){
    struct tm* dt = localtime(&spcm->date);
    
    printf("\033[32m[%02d.%02d.%d %02d:%02d](%s@%s): %s\033[0;37m\n", 
    dt->tm_mday, dt->tm_mon+1, dt->tm_year+1900, dt->tm_hour, dt->tm_min,
    spcm->username, spcm->channelName, spcm->message);
}

int sendPrivateMessage(int id, int userId, char* receiver, char* mess){
    int len = strlen(mess);
    int n = (len-1)/MAX_MESSAGE_LENGTH + 1;
    int i;
    message msg;
    char* helper;
    int ret = 0;
    for(i=0; i<n; i++){
        helper = substring(mess, i*len, (i+1)*len);
        getClientPrivateMessage(&msg, userId, receiver, time(NULL), helper);
        if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0) ret = -1;
        free(helper);
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        printPrivateMessage(&msg.data.spm);
    }
    return ret;
}

void printPrivateMessage(server_private_message* spm){
    struct tm* dt = localtime(&spm->date);
    
    printf("\033[32m[%02d.%02d.%d %02d:%02d](%s->%s): %s\033[0;37m\n", 
    dt->tm_mday, dt->tm_mon+1, dt->tm_year+1900, dt->tm_hour, dt->tm_min,
    spm->sender, spm->receiver, spm->message);
}

void handleHeartbeat(int id, int userId){
    message msg;
    int type = getHeartbeatReceiveTypeToHandleMessages(userId);
    while(1){
        if(msgrcv(id, &msg, sizeof(msg.data), type, 0) != -1){
            switch(msg.data.sm.code){
                case 0:
                    printf("\033[1;31mWyslij dowolna wiadomosc lub komende (np. \"/zyje\"), aby potwierdzic serwerowi, ze jestes aktywny!\033[0;37m\n");
                break;
                case 1:
                    printf("\033[1;31mZostales wylogowany z powodu nieaktywnosci!\033[0;37m\n");
                    kill(0, SIGUSR1);
                break;
            }
        }else break;
    }
}

void sendHeartbeat(int id, int userId){
    message msg;
    getClientSendHeatbeatMessage(&msg, userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) == -1){
        printf("\033[1;31mNie udalo sie wyslac sygnalu o aktywnosci!\n033[0;37m");
    }
}

void getChannels(int id, int userId, char channels[MAX_CHANNEL_NAME_LENGTH+1][MAX_CHANNELS_NUMBER], int* channelsLength){
    message msg;
    msgrcv(id, &msg, sizeof(msg.data), userId, 0);
    int i, n=msg.data.slbm.size;
    for(i=0; i<n; i++){
        msgrcv(id, &msg, sizeof(msg.data), userId, 0);
        strcpy(channels[*channelsLength], msg.data.sleocm.channel);
        (*channelsLength)++;
    }
}


