// Aplikacja serwera

#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "messages.h"
#include "servers_config.h"
#include "user.h"
#include "channel.h"
#include "list.h"

// stałe

// id użytkownika, który jako pierwszy się zaloguje. Należy pamiętać, że komunikacja serwer-klient odbywa się za pomoca tego id
#define FIRST_USER_ID 1000

// 1 oznacza wyświetlanie logów, 0 oznacza niewyświetlanie
const int LOGS = 1;



// deklaracje metod zarządzania serwerem

// prosta funkcja tworząca kolejkę IPC
// może zostać rozwinięta, gdy dojdzie nam wiele serwerów
// zwraca -1 w razie niepowodzenia lub id kolejki
int startServer(int serverid);

// kończy pracę serwera, gdy na wejściu podany zostanie znak 'q'
// zamyka kolejkę i zabija wszystkie procesy
void stopServer(int id);

// rozpoczyna pętle programu, czyli obsługę eventów od klientów
void startLoop();

// pomocnicza funkcja do zabijania procesów potomnych
void stopProcess();



// deklaracje metod obsługi zdarzeń (eventów)

// obsługa logowania użytkownika
// zwraca -1, gdy mamy limit użytkowników
// zwraca -2, gdy nazwa nie jest unikalna
// zwraca 0, jeżeli się uda
int handleNewUsersEvent(int id, client_enter_message* cem, Channel* channels, int channelsLength, User* users, int* usersLength);

// obsługa wylogowania użytkownika
void removeUserEvent(user_message* um, User* users, int* usersLength);

// obsługa wyświetlania listy użytkowników
void sendUsersList(int id, int userId, User* users, int usersLength);

// obsługa wyświetlania listy kanałów
void sendChannelsList(int id, int userId, Channel* channels, int channelsLength);

// obsługa wyświetlania listy kanałów
void sendChannelsWithUsersList(int id, int userId, Channel* channels, int channelsLength);

// dodanie użytkownika do kanału - jeżeli kanał nie istnieje, zostanie dodany
// zwraca -1 jeżeli użytkownik nie istnieje
// zwraca -2 jeżeli użytkownik już należy do tego kanału
// zwraca -3 jeżeli nie da się już stworzyć nowego kanału, a jest taka potrzeba
// zwraca 0 jeżeli się uda
int addUserToChannel(int id, client_channel_message* ccm, Channel* channels, int* channelsLength, User* users, int usersLength);

// usuwanie użytkownika z kanału - kanał nigdy nie jest usuwany
// zwraca -1 jeżeli użytkownik nie istnieje
// zwraca -2 jeżeli kanał nie istnieje
// zwraca -3 jeżeli użytkownik nie należy do tego kanału
// zwraca -4 jeżeli użytkownik próbuje opuścić kanał publiczny
// zwraca 0 jeżeli się uda
int removeUserFromChannel(int id, client_channel_message* ccm, Channel* channels, int* channelsLength, User* users, int usersLength);

// wysyłanie wiadomości publicznej do wszystkich ludzi z kanału
// zwraca -1 jeżeli kanał nie istnieje
// zwraca -2 jeżeli nadawca nie istnieje
// zwraca -3 jeżeli co najmniej jedna osoba nie otrzymała wiadomości
// zwraca 0 jeżeli się uda
int sendPublicChannelMessage(int id, client_public_channel_message* cpcm, Channel* channels, int channelsLength);

// wysyłanie wiadomości prywatnej
// zwraca -1 jeżeli odbiorca nie istnieje
// zwraca -2 jeżeli nadawca nie istnieje
// zwraca -3 jeżeli odbiorca jest offline
// zwraca -4 jeżeli nie udało się wysłać komunikatu
// zwraca 0 jeżeli się uda
int sendPrivateMessage(int id, client_private_message* cpm, User* users, int usersLength);

// wysyłanie 10 ostatnich wiadomości po zalogowaniu
void sendLastMessages(int id, User* user);

// funkcja, która działa jako proces potomny i wysyła prośby oraz otrzymuje sygnały o życiu
// jeżeli przez odpowiedni czas nie otrzyma takiej informacji wysyła do serwera komunikat, który wyloguje użytkownika
void handleHeartbeat(int id, int userId);

// funkcja główna, zarządzająca
int main(int argc, char *argv[]) {

    int serverId;
    if(argc != 2){
        printf("Musisz podac id serwera jako pierwszy argument!\n");
        exit(-1);
    }else{
        serverId = atoi(argv[1]);
    }
    
    signal(SIGUSR1, stopProcess);

    int id = startServer(serverId);
    if(id == -1){
        printf("Blad - serwer nie zostanie uruchomiony!\n");
        exit(-1);
    }

    printf("Serwer uruchomiony.  (aby wylaczyc wpisz \"q\")\n");

    if(fork() == 0)startLoop(id);
    else stopServer(id);
    
    return 0;
}


// definicje metod zarządzania serwerem

int startServer(int serverId){return msgget(serverId, IPC_FLAGS);}

void stopServer(int id){
    char c;
    do{
        scanf("%c", &c);
    }while(c != 'q');
    
    if(msgctl(id, IPC_RMID, 0) == -1 && LOGS) printf("Nie udalo sie zamknac serwera\n");
    kill(0, SIGUSR1);
}

void startLoop(int id){
    User users[MAX_USERS_NUMBER];
    int usersLength = 0;

    Channel channels[MAX_CHANNELS_NUMBER];
    strcpy(channels[0].name, "Publiczny");
    initLastMessagesList(&channels[0].lastMessages, LAST_MESSAGES_LENGTH);
    channels[0].usersLength = 0;
    int channelsLength = 1;

    while(1){
        message msg;
        msgrcv(id, &msg, sizeof(msg.data), -FIRST_USER_ID+1, 0);
        switch(msg.type){
        case CLIENT_ENTER_MESSAGE_TYPE:
            if(LOGS) printf("Probuje dodac nowego uzytkownika %s\n", msg.data.cem.username);
            handleNewUsersEvent(id, &msg.data.cem, channels, channelsLength, users, &usersLength);
            break;
        case CLIENT_LEAVE_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld probuje wyjsc z serwera\n", msg.data.um.id);
            removeUserEvent(&msg.data.um, users, &usersLength);
            break;
        case CLIENT_LIST_OF_USERS_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld prosi o liste uzytkownikow\n", msg.data.um.id);
            sendUsersList(id, msg.data.um.id, users, usersLength);
            break;
        case CLIENT_LIST_OF_CHANNELS_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld prosi o liste kanalow\n", msg.data.um.id);
            sendChannelsList(id, msg.data.um.id, channels, channelsLength);
            break;
        case CLIENT_JOIN_CHANNEL_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld chce dolaczyc na kanal %s\n", msg.data.ccm.userid, msg.data.ccm.channelName);
            addUserToChannel(id, &msg.data.ccm, channels, &channelsLength, users, usersLength);
            break;
        case CLIENT_LIST_OF_CHANNELS_WITH_USERS_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld prosi o rozszerzona liste kanalow\n", msg.data.um.id);
            sendChannelsWithUsersList(id, msg.data.um.id, channels, channelsLength);
            break;
        case CLIENT_LEAVE_CHANNEL_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik o id %ld chce wyjsc z kanalu %s\n", msg.data.ccm.userid, msg.data.ccm.channelName);
            removeUserFromChannel(id, &msg.data.ccm, channels, &channelsLength, users, usersLength);
            break;
        case CLIENT_PUBLIC_CHANNEL_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik od id %ld chce wyslac na kanal %s wiadomosc \"%s\"\n", msg.data.cpcm.userid, msg.data.cpcm.channelName, msg.data.cpcm.message);
            sendPublicChannelMessage(id, &msg.data.cpcm, channels, channelsLength);
            break;
        case CLIENT_PRIVATE_MESSAGE_TYPE:
            if(LOGS) printf("Uzytkownik od id %ld chce wyslac do %s prywatna wiadomosc \"%s\"\n", msg.data.cpm.userid, msg.data.cpm.receiver, msg.data.cpm.message);
            sendPrivateMessage(id, &msg.data.cpm, users, usersLength);
            break;
        case SERVER_NO_HEARTBEAT_MESSAGE_TYPE: // ta wiadomość przychodzi od procesu potomnego serwera
            if(LOGS) printf("Uzytkownik o id %ld nie dawal sygnalu o zyciu, wiec zostanie wylogowany\n", msg.data.um.id);
            removeUserEvent(&msg.data.um, users, &usersLength);
            break;
        default:
            printf("Otrzymano wiadomosc o nieznanym typie %ld\n", msg.type);
        }
    }
}

void stopProcess(){
    if(LOGS) printf("Zamykam %d\n", getpid());
    exit(0);
}



// definicje metod obsługi zdarzeń (eventów)

int handleNewUsersEvent(int id, client_enter_message* cem, Channel* channels, int channelsLength, User* users, int* usersLength){
    Channel* public = &channels[0];
    message response;
    response.type = SERVER_ENTER_MESSAGE_TYPE;
    server_enter_message* sem = &response.data.sem;
    strcpy(sem->username, cem->username);
    strcpy(sem->channelName, public->name);
    User* user = findUserByName(*usersLength, users, cem->username);
    if(user == NULL){ // unique username, new client
        if(*usersLength < MAX_USERS_NUMBER){
            strcpy(users[*usersLength].username, cem->username);
            if(*usersLength == 0) users[0].userId = FIRST_USER_ID;
            else users[*usersLength].userId = users[*usersLength-1].userId+1;
            users[*usersLength].channels[0] = public;
            users[*usersLength].channelsLength = 1;
            users[*usersLength].isSignedIn = 1;
            public->users[public->usersLength++] = &users[*usersLength];
            sem->id = users[*usersLength].userId;
            user = &users[*usersLength];
            (*usersLength)++;
        }else{
            sem->id = -1;
        }
    }else if(user->isSignedIn == 1){
        sem->id = -2;
    }else{ // try sign in again
        user->isSignedIn = 1;
        sem->id = user->userId;
    }


    if(msgsnd(id, &response, sizeof(response.data), 0) != 0){
        printf("Nie udalo mi sie poinformowac uzytkownika %s o jego rejestracji!\n", sem->username);
    }
    
    if(sem->id >= 0){
        
        // send channels to user
        message msg;
        msg.type = user->userId;
        server_list_begin_message* slbm = &msg.data.slbm;
        slbm->size = user->channelsLength-1;
        if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
            if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %ld dlugosci listy kanalow\n", user->userId);
        }else{
            server_list_element_of_channels_message* sleocm = &msg.data.sleocm;
            int i;
            for(i=1; i<user->channelsLength; i++){
                strcpy(sleocm->channel, user->channels[i]->name);
                if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
                    if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %ld kanalu %s podczas logowania\n", user->userId, channels[i].name);
                }
            }
        }

        sendLastMessages(id, user);
        
        int f = fork();
        if(f == 0){
            handleHeartbeat(id, user->userId);
            exit(0);
        }else{
            user->heartbeatProcess = f;
        }
    }

    return sem->id < 0 ? sem->id : 0;
}

void removeUserEvent(user_message* um, User* users, int* usersLength){
    int i;
    User* toRemove = findUser(um->id, users, *usersLength);
    toRemove->isSignedIn = 0;
    kill(toRemove->heartbeatProcess, SIGKILL);
}

void sendUsersList(int id, int userId, User* users, int usersLength){
    message msg;
    msg.type = userId;
    server_list_begin_message* slbm = &msg.data.slbm;
    slbm->size = usersLength;
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d wielkosci listy uzytkownikow\n", userId);
    }else{
        server_list_element_of_users_message* sleoum = &msg.data.sleoum;
        int i;
        for(i=0; i<usersLength; i++){
            strcpy(sleoum->username, users[i].username);
            sleoum->online = users[i].isSignedIn;
            if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
                if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d uzytkownika %s podczas prosby o liste uzytkownikow\n", userId, users[i].username);
            }
        }
    }   
}

void sendChannelsList(int id, int userId, Channel* channels, int channelsLength){
    message msg;
    msg.type = userId;
    server_list_begin_message* slbm = &msg.data.slbm;
    slbm->size = channelsLength;
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d wielkosci listy kanalow\n", userId);
    }else{
        server_list_element_of_channels_message* sleocm = &msg.data.sleocm;
        int i;
        for(i=0; i<channelsLength; i++){
            strcpy(sleocm->channel, channels[i].name);
            if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
                if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d kanalu %s podczas prosby o liste kanalow\n", userId, channels[i].name);
            }
        }
    }    
}

void sendChannelsWithUsersList(int id, int userId, Channel* channels, int channelsLength){
    message msg;
    msg.type = userId;
    server_list_begin_message* slbm = &msg.data.slbm;
    slbm->size = channelsLength;
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d wielkosci listy kanalow\n", userId);
    }else{
        SimpleChannel* channel = &msg.data.sleocwum.channel;
        int i, j;
        for(i=0; i<channelsLength; i++){
            strcpy(channel->name, channels[i].name);
            channel->usersLength = channels[i].usersLength;
            for(j=0; j<channels[i].usersLength; j++){
                strcpy(channel->users[j], channels[i].users[j]->username);
            }
            if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
                if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %d kanalu %s podczas prosby o rozszerzona liste kanalow\n", userId, channels[i].name);
            }
        }
    }    
}

int addUserToChannel(int id, client_channel_message* ccm, Channel* channels, int* channelsLength, User* users, int usersLength){
    int val;

    int i;
    for(i=0; i<*channelsLength; i++){
        if(strcmp(ccm->channelName, channels[i].name) == 0){
            break;
        }
    }

    message msgs[LAST_MESSAGES_LENGTH];
    int msgLength = 0;


    User* user = findUser(ccm->userid, users, usersLength);
    if(user == NULL) val = -1;
    else{
        if(i != *channelsLength){ // found
            if(hasChannel(user, &channels[i]) == 1) val = -2;
            else{
                channels[i].users[channels[i].usersLength++] = user;
                user->channels[user->channelsLength++] = &channels[i];
                val = 0;

                last_messages_list_element* le = channels[i].lastMessages.front;
                int type = getPublicMessageTypeToHandleMessages(user->userId);
                while(le != NULL){
                    msgs[msgLength].type = type;
                    server_public_channel_message* spcm = &msgs[msgLength].data.spcm;
                    strcpy(spcm->channelName, channels[i].name);
                    spcm->date = le->data.date;
                    strcpy(spcm->username, le->data.sender);
                    strcpy(spcm->message, le->data.message);
                    msgLength++;
                    le = le->next;
                }
            }
        }else{ // add new
            if(*channelsLength == MAX_CHANNELS_NUMBER) val = -3;
            channels[*channelsLength].usersLength = 1;
            strcpy(channels[*channelsLength].name, ccm->channelName);
            channels[*channelsLength].users[0] = user;
            user->channels[user->channelsLength++] = &channels[*channelsLength];
            initLastMessagesList(&channels[*channelsLength].lastMessages, LAST_MESSAGES_LENGTH);
            (*channelsLength)++;
            val = 0;
        }
    }

    message msg;
    msg.type = ccm->userid;
    msg.data.sm.code = val;

    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %ld odpowiedzi na dolaczenie na kanal\n", ccm->userid);
    }else if(msgLength > 0){
        for(i=0; i<msgLength; i++){
            if(msgsnd(id, &msgs[i], sizeof(msgs[i].data), 0) != 0){
                if(LOGS) printf("Uzytkownik o id %ld nie otrzyma starej wiadomosci \"%s\" z kanalu %s\n", user->userId, msgs[i].data.spcm.message, msgs[i].data.spcm.channelName);
            }
        }
    }
    return val;
}

int removeUserFromChannel(int id, client_channel_message* ccm, Channel* channels, int* channelsLength, User* users, int usersLength){
    int val;

    User* user = findUser(ccm->userid, users, usersLength);
    if(user == NULL) val = -1;
    else{
        int i;
        for(i=0; i<*channelsLength; i++){
            if(strcmp(ccm->channelName, channels[i].name) == 0){
                break;
            }
        }


        if(i == *channelsLength) val = -2; // channel not exists
        else if(i == 0) val = -4; // public channel can't be leaved
        else{
            int j, k;
            for(j=0; j<channels[i].usersLength; j++){
                if(ccm->userid == channels[i].users[j]->userId){
                    // remove user from channel
                    for(k=j; k<channels[i].usersLength-1; k++){
                        channels[i].users[k] = channels[i].users[k+1];
                    }
                    removeChannel(user, &channels[i]);
                    val = 0;
                    break;
                }
            }
            if(j == channels[i].usersLength) val = -3;
            else channels[i].usersLength--;
        }
    }

    message msg;
    msg.type = ccm->userid;
    msg.data.sm.code = val;

    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Nie udalo mi sie wyslac uzytkownikowi o id %ld odpowiedzi na wyjscie z kanalu\n", ccm->userid);
    }

    return val;
}

int sendPublicChannelMessage(int id, client_public_channel_message* cpcm, Channel* channels, int channelsLength){
    Channel* channel = findChannel(cpcm->channelName, channels, channelsLength);
    if(channel == NULL){
        if(LOGS) printf("Nie udalo mi sie znalezc kanalu\n");
        return -1;
    }
    User* sender = hasUserById(cpcm->userid, channel);
    if(sender == NULL){
        if(LOGS) printf("Nie udalo mi sie znalezc uzytkownika\n");
        return -2;
    }

    message msg;
    server_public_channel_message* spcm = &msg.data.spcm;
    spcm->date = cpcm->date;
    strcpy(spcm->channelName, cpcm->channelName);
    strcpy(spcm->username, sender->username);
    strcpy(spcm->message, cpcm->message);
    int i;
    int any = 0;
    for(i=0; i<channel->usersLength; i++){
        if(channel->users[i]->userId != cpcm->userid && channel->users[i]->isSignedIn == 1){
            msg.type = getPublicMessageTypeToHandleMessages(channel->users[i]->userId);
            if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
                if(LOGS) printf("Uzytkownik o id %ld nie otrzyma wiadomosci \"%s\" na kanal %s\n", channel->users[i]->userId, cpcm->message, cpcm->channelName);
                any = 1;
            }
        }
    }
    msg.type = cpcm->userid;
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Uzytkownik o id %ld nie otrzyma potwierdzenia wyslania wiadomosci\n", cpcm->userid);
        any = 1;
    }

    user_channel_message um;
    strcpy(um.message, spcm->message);
    strcpy(um.sender, spcm->username);
    um.date = spcm->date;
    addElementToLastMessagesList(&channel->lastMessages, &um);

    if(any) return -3;
    return 0;
}

int sendPrivateMessage(int id, client_private_message* cpm, User* users, int usersLength){
    User* receiver = findUserByName(usersLength, users, cpm->receiver);
    if(receiver == NULL){
        if(LOGS) printf("Nie udalo mi odnalezc odbiorcy\n");
        return -1;
    }

    if(!receiver->isSignedIn){
        if(LOGS) printf("Nadawca jest offline\n");
        return -3;
    }

    User* sender = findUser(cpm->userid, users, usersLength);
    if(sender == NULL){
        if(LOGS) printf("Nie udalo mi sie znalezc nadawcy\n");
        return -2;
    }

    message msg;
    server_private_message* spm = &msg.data.spm;
    spm->date = cpm->date;
    strcpy(spm->receiver, cpm->receiver);
    printf("%s\n", sender->username);
    strcpy(spm->sender, sender->username);
    strcpy(spm->message, cpm->message);
    int i;
    int any = 0;

    msg.type = getPrivateMessageTypeToHandleMessages(receiver->userId);
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Odbiorca nie otrzyma wiadomosci prywatnej od nadawcy\n");
        any = 1;
    }
     
    msg.type = cpm->userid;
    if(msgsnd(id, &msg, sizeof(msg.data), 0) != 0){
        if(LOGS) printf("Uzytkownik o id %ld nie otrzyma potwierdzenia wyslania wiadomosci\n", cpm->userid);
        any = 1;
    }
    if(any) return -4;
    return 0;
}

void sendLastMessages(int id, User* user){
    int i;
    last_messages_list_element* le;
    message msg[user->channelsLength*LAST_MESSAGES_LENGTH];
    int msgLength = 0;
    int type = getPublicMessageTypeToHandleMessages(user->userId);
    for(i=0; i<user->channelsLength; i++){
        le = user->channels[i]->lastMessages.front;
        while(le != NULL){
            msg[msgLength].type = type;
            server_public_channel_message* spcm = &msg[msgLength].data.spcm;
            strcpy(spcm->channelName, user->channels[i]->name);
            spcm->date = le->data.date;
            strcpy(spcm->username, le->data.sender);
            strcpy(spcm->message, le->data.message);
            msgLength++;
            le = le->next;
        }
    }

    // sortowanie datą (algorytm sortowania bąbelkowego)
    int j;
    message temp;
    for(i=0; i<msgLength-1; i++){
        for(j=0; j<msgLength-i-1; j++){
            if(msg[j].data.spcm.date > msg[j+1].data.spcm.date){
                temp = msg[j];
                msg[j] = msg[j+1];
                msg[j+1] = temp;
            }
        }
    }

    // sending
    for(i=0; i<msgLength; i++){
        if(msgsnd(id, &msg[i], sizeof(msg[i].data), 0) != 0){
            if(LOGS) printf("Uzytkownik o id %ld nie otrzyma starej wiadomosci \"%s\" z kanalu %s\n", user->userId, msg[i].data.spcm.message, msg[i].data.spcm.channelName);
        }
    }
}

void handleHeartbeat(int id, int userId){
    // zacznij dziecko:
    // czeka
    // wysyla, ze slabo
    // czeka
    // konczy

    // rodzic odbiera, jezeli odbierze konczy dziecko i zaczyna nowe dziecko
    int child;
    int send = getHeartbeatSendTypeToHandleMessages(userId), receive = getHeartbeatReceiveTypeToHandleMessages(userId);
    int parent = getpid();
    while(1){
        child = fork();
        // jeżeli użytkownik się wyloguje to proces każdego dziecka i rodzica tego procesu musi się zakończyć
        // aby zakończyć proces dzieci zabijamy rodzica, a dziecko sprawdza czy rodzic żyje, jeżeli nie to kończy
        if(child == 0){
            sleep(HEARTBEAT_SECONDS);
            if(getppid() != parent) exit(0);
            message msg;
            msg.type = receive;
            msg.data.sm.code = 0;
            msgsnd(id, &msg, sizeof(msg.data), 0);
            sleep(HEARTBEAT_WAIT_SECONDS);
            if(getppid() != parent) exit(0);
            msg.data.sm.code = 1;
            msgsnd(id, &msg, sizeof(msg.data), 0);
            msg.type = SERVER_NO_HEARTBEAT_MESSAGE_TYPE;
            msg.data.um.id = userId;
            msgsnd(id, &msg, sizeof(msg.data), 0);
            exit(0);
        }else{
            message msg;
            msgrcv(id, &msg, sizeof(msg.data), send, 0);
            kill(child, SIGKILL);
        }
    }
}


