#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "messages.h"


int key = 2137;
buf_message mb;
buf_message mb_back;


struct User {
	char login[NAME_SIZE];
	int groups[DEF_SIZE];
	int online;
};

static int id;

void closeServer() {
	printf("\nClosing server...\n");
	msgctl(id, IPC_RMID, 0);
	exit(0);
}

int main(int argc, char* argv[]) {
	struct User users[DEF_SIZE];
	int users_count = 0;
	
	id = msgget(key, IPC_CREAT | 0666);
	signal(SIGINT, closeServer);

	while (1) {
		msgrcv(id, &mb, MSG_SIZE, 0, 0);

		if (mb.type < 100) {
			int found = 0;
			switch(mb.type) {

				case 99:		// user logout
					for (int i=0; i<=DEF_SIZE; i++)
						if (!strcmp(users[i].login, mb.text))
							users[i].online = 0;
					break;
				case 98:	;	// user login
					for (int i=0; i<=DEF_SIZE; i++) {
						if (!strcmp(users[i].login, mb.text)) {
							found = 1;
							mb_back.text[0] = i;
							users[i].online = 1;
						}
					}

					if (found) {
						mb_back.type = 199;
						printf("Server: old user logging in - %s - %d\n", mb.text, mb_back.text[0]);
					}
					else {
						mb_back.text[0] = users_count;
						mb_back.type = 198;
						strcpy(users[users_count].login, mb.text);
						users[users_count].online = 1;
						users_count++;
						printf("Server: new user logging in - %s - %d\n", mb.text, mb_back.text[0]);
					}

					msgsnd(id, &mb_back, MSG_SIZE, 0);
					break;
				case 1:
					mb_back.type = 101;
					mb_back.text[0] = users_count;
					msgsnd(id, &mb_back, MSG_SIZE, IPC_NOWAIT);
					for (int i=0; i<users_count; i++) {
						strcpy(mb_back.text, users[i].login);
						printf("%s\n", users[i].login);
						msgsnd(id, &mb_back, MSG_SIZE, IPC_NOWAIT);
						printf("2\n");
					}
			}
		}
	}
}