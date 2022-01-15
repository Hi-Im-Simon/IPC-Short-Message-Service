#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "messages.h"

int key = 2137;
buf_message mb;
buf_message mb_back;

char username[NAME_SIZE];
int logged_in = 0;


int logIn(int* id) {
	printf("\nPlease, provide a username: ");

    scanf("%s", mb.mtext);
	mb.mtype = 1;
	strcpy(username, mb.mtext);

	msgsnd(*id, &mb, strlen(mb.mtext) + 1, IPC_NOWAIT);

	return 1;
}

int menu() {
	char choice[MSG_SIZE];

	printf("Available commands:\n");
	printf("[users] - Check who is currently logged in\n");
	printf("[groups] - Check all available groups and their parcipants\n");
	printf("[join] - Join one of available group\n");
	printf("[leave] - Leave a group\n");
	printf("[msg] - Send a message to a user\n");
	printf("[msggroup] - Send a message to a group\n");
	printf("[msgview] - View your message box\n");
	printf("[logout]\n");
	printf("What would you like to do?: ");

	scanf("%s", choice);

	if (!strcmp(choice, "users"))
		return 0;
	else if (!strcmp(choice, "logout") || !strcmp(choice, "exit") || !strcmp(choice, "x")) {
		logged_in = 0;
		return 1;
	}
	else {
		printf("\n### Wrong command, try again! ###\n");
		menu();
	}
}

int main(int argc, char* argv[])  {
	int id = msgget(key, IPC_CREAT | 0666);

	while (1) {
		if (!logged_in) {
			logged_in = logIn(&id);
		}
		else {
			if (menu()) continue;
		}

		msgrcv(id, &mb_back, MSG_SIZE, 0, 0);

		if (mb_back.mtype >= 100) {
			if (mb_back.mtype == 101) {
				printf("\n~~~ Welcome back, %s! ~~~\n", username);
			}
			else if (mb_back.mtype == 102) {
				printf("\n~~~ Hello, %s! ~~~\n", username);
			}
		}		
	}
}