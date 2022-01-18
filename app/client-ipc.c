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
int user_id;
int logged_in = 0;


int logIn(int* id) {
	printf("\nPlease, provide a username: ");

    scanf("%s", mb.text);
	mb.type = 98;
	strcpy(username, mb.text);

	msgsnd(*id, &mb, strlen(username) + 1, IPC_NOWAIT);

	return 1;
}

int logOut(int* id) {
	mb.type = 99;
	strcpy(username, mb.text);

	msgsnd(*id, &mb, strlen(username) + 1, IPC_NOWAIT);

	return 0;
}

int menu(int* id) {
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

	
	if (!strcmp(choice, "logout") || !strcmp(choice, "exit") || !strcmp(choice, "x"))
		logged_in = logOut(id);
	else if (!strcmp(choice, "users")) {
		mb.type = 1;
		msgsnd(*id, &mb, 0, IPC_NOWAIT);
	}
	else if (!strcmp(choice, "groups"))
		return 2;
	else if (!strcmp(choice, "join"))
		return 3;
	else if (!strcmp(choice, "leave"))
		return 4;
	else if (!strcmp(choice, "msg"))
		return 5;
	else if (!strcmp(choice, "msggroup"))
		return 6;
	else if (!strcmp(choice, "msgview"))
		return 7;
	else {
		printf("\n### Wrong command, try again! ###\n");
		menu(id);
	}
}

int main(int argc, char* argv[])  {
	int id = msgget(key, IPC_CREAT | 0666);

	while (1) {
		if (!logged_in)
			logged_in = logIn(&id);
		else {
			menu(&id);

			if (!logged_in)	// if user decided to logout, go back to logging phase
				continue;
		}

		msgrcv(id, &mb_back, MSG_SIZE, 0, 0);

		if (mb_back.type >= 100) {
			if (mb_back.type == 199) {
				user_id = mb_back.text[0];
				printf("\n~~~ Welcome back, %s! ~~~\n", username);
			}
			else if (mb_back.type == 198) {
				user_id = mb_back.text[0];
				printf("\n~~~ Hello, %s! ~~~\n", username);
			}
			else if (mb_back.type == 101) {
				int users_count = mb_back.text[0];
				for (int _=0; _<users_count; _++) {
					printf("1\n");
					msgrcv(id, &mb_back, MSG_SIZE, 0, 0);
					printf("2\n");
					printf("%s\n", mb_back.text);
				}

			}
		}		
	}
}