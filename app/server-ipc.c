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

struct User {
	char login[NAME_SIZE];
	int groups[DEF_SIZE];
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

	int key = 2137;
	buf_message mb;
	buf mb_back;
	
	id = msgget(key, IPC_CREAT | 0666);
	signal(SIGINT, closeServer);

	while (1) {
		msgrcv(id, &mb, MSG_SIZE, 0, 0);

		if (mb.mtype < 100) {
			switch(mb.mtype) {
				case 1: ;	// semi-colon to fix a compiler bug
					int found = 0;
					for (int i=0; i<=DEF_SIZE; i++) {
						if (!strcmp(users[i].login, mb.mtext)) {
							found = 1;
							int user_id = i;
						}
					}

					if (found) {
						mb_back.mtype = 101;
						printf("Server: old user logging in - %s\n", mb.mtext);
					}
					else {
						mb_back.mtype = 102;
						strcpy(users[users_count++].login, mb.mtext);
						printf("Server: new user logging in - %s\n", mb.mtext);
					}

					msgsnd(id, &mb_back, MSG_SIZE, 0);
					break;
			}
		}
	}
}