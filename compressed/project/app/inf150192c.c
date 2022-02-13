#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>


#include "inf150192s_messages.h"


int id;
char username[NAME_SIZE];


void send_msg_type(int type) {
	msg_test mb_test;
	mb_test.type = type;

	msgsnd(id, &mb_test, 0, IPC_NOWAIT);
	msgrcv(id, &mb_test, MSG_SIZE, 0, 0);
	sleep(0.01);
}


void print_menu() {
    printf("Available commands:\n");
	printf("[users] - Check who is currently logged in\n");
	printf("[groups] - Check all available groups and their parcipants\n");
	printf("[join] - Join one of available groups\n");
	printf("[leave] - Leave a group\n");
	printf("[msg] - Send a message to a user\n");
	printf("[msggroup] - Send a message to a group\n");
	printf("[msgview] - View your message box\n");
	printf("[logout]\n");
	printf("[exit]\n");
}

void log_in() {
    msg_txt mb;
	mb.type = 1;
	msg_int mb_back;

    while (1) {
        printf("Please, provide a username: ");
        scanf("%s", username);
		strcpy(mb.text, username);

		send_msg_type(1);
		msgsnd(id, &mb, strlen(mb.text) + 1, IPC_NOWAIT);
		msgrcv(id, &mb_back, MSG_SIZE, 0, 0);

		switch (mb_back.value) {
		case 1:
			printf("\n~~~ Welcome back, %s! ~~~\n", username);
			return;
		
		case 2:
			printf("\n~~~ Hello, %s! ~~~\n", username);
			return;
		case 0:
			printf("\n### User %s is already logged in! Select a different username... ###\n", username);
			break;
		}
    }
}

void log_out() {
	msg_txt mb;
	mb.type = 2;
	msg_test mb_back;

	strcpy(mb.text, username);

	send_msg_type(2);
	msgsnd(id, &mb, strlen(mb.text) + 1, IPC_NOWAIT);
	msgrcv(id, &mb_back, MSG_SIZE, 0, 0);
}

void show_users() {
	msg_test mb;
	mb.type = 3;
	msg_int mb_back;

	send_msg_type(3);
	msgsnd(id, &mb, 0, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	int users_online_count = mb_back.value;
	printf("There are currently %d users online:\n", users_online_count);

	msg_txt mb_back2;

	for (int i=0; i<users_online_count; i++) {
		msgsnd(id, &mb, 0, IPC_NOWAIT);
		msgrcv(id, &mb_back2, MSG_SIZE, 0, 0);

		if (strcmp(mb_back2.text, username))
			printf("- %s\n", mb_back2.text);
		else
			printf("- %s (you)\n", mb_back2.text);
	}
}

void show_groups() {
	msg_test mb;
	mb.type = 4;
	msg_int mb_back;

	send_msg_type(4);
	msgsnd(id, &mb, 0, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	int groups_count = mb_back.value;
	printf("There are currently %d groups:\n", groups_count);

	msg_txt mb_back2;

	for (int i=0; i<groups_count; i++) {
		msgsnd(id, &mb, 0, IPC_NOWAIT);
		msgrcv(id, &mb_back, 4, 0, 0);
		msgsnd(id, &mb, 0, IPC_NOWAIT);
		msgrcv(id, &mb_back2, MSG_SIZE, 0, 0);

		printf("- NAME: %s, USERS:", mb_back2.text);
		
		if (mb_back.value > 0) {
			for (int j=0; j<mb_back.value; j++) {
				msgsnd(id, &mb, 0, IPC_NOWAIT);
				msgrcv(id, &mb_back2, MSG_SIZE, 0, 0);

				printf(" %s", mb_back2.text);
			}
		}
		else
			printf(" -");
		printf("\n");
	}
}

void add_to_group() {
	msg_txt mb;
	mb.type = 5;
	msg_int mb_back;

	printf("Which group would you like to join?: ");
	scanf("%s", mb.text);

	send_msg_type(5);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, username);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	if (mb_back.value == 1) 
		printf("Successfully joined a new group!\n");
	else if (mb_back.value == 2)
		printf("You are already in this group!\n");
	else
		printf("This group doesn't exist!\n");
}

void remove_from_group() {
	msg_txt mb;
	mb.type = 6;
	msg_int mb_back;

	printf("Which group would you like to leave?: ");
	scanf("%s", mb.text);

	send_msg_type(6);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, username);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	if (mb_back.value == 1) 
		printf("Successfully left a group!\n");
	else if (mb_back.value == 2)
		printf("You are not a member of this group!\n");
	else
		printf("This group doesn't exist!\n");
}

void send_msg_to_user() {
	msg_txt mb;
	mb.type = 7;
	msg_int mb_back;
	char message[MSG_SIZE];

	printf("Which user would you like to send a message to?: ");
	scanf("%s", mb.text);

	if (!strcmp(mb.text, username)) {
		printf("You can't send a message to yourself!\n");
		return;
	}

	printf("Your message:\n> ");
	scanf(" %[^\n]", message);

	send_msg_type(7);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, username);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, message);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	if (mb_back.value)
		printf("Successfully sent a message!\n");
	else
		printf("This user doesn't exist!\n");
}

void send_msg_to_group() {
	msg_txt mb;
	mb.type = 8;
	msg_int mb_back;
	char message[MSG_SIZE];

	printf("Which group would you like to send a message to?: ");
	scanf("%s", mb.text);

	printf("Your message:\n> ");
	scanf(" %[^\n]", message);

	send_msg_type(8);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, username);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 0, 0, 0);
	strcpy(mb.text, message);
	msgsnd(id, &mb, MSG_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);

	if (mb_back.value == 1)
		printf("Successfully sent a message!\n");
	else if (mb_back.value == 2)
		printf("You are not a member of this group!\n");
	else
		printf("This group doesn't exist!\n");
}

void view_msgs() {
	msg_txt mb;
	mb.type = 9;
	msg_int mb_back;

	strcpy(mb.text, username);

	send_msg_type(9);
	msgsnd(id, &mb, NAME_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);
	int user_msgs_count = mb_back.value;
	msgsnd(id, &mb, NAME_SIZE, IPC_NOWAIT);
	msgrcv(id, &mb_back, 4, 0, 0);
	int group_msgs_count = mb_back.value;

	printf("You have %d message(s):\n", (user_msgs_count + group_msgs_count));

	msg_txt mb_back2;

	for (int i=0; i<user_msgs_count; i++) {
		msgsnd(id, &mb, 0, IPC_NOWAIT);
		msgrcv(id, &mb_back2, MSG_SIZE, 0, 0);

		printf("> %s\n", mb_back2.text);
	}

	for (int j=0; j<group_msgs_count; j++) {
		msgsnd(id, &mb, 0, IPC_NOWAIT);
		msgrcv(id, &mb_back2, MSG_SIZE, 0, 0);

		printf("> [%s\n", mb_back2.text);
	}
}

int main(int argc, char* argv[]) {
	id = msgget(KEY, IPC_CREAT | 0666);
	int show_menu = 1;

	// logging loop
	while (1) {
		log_in();

		// action loop
		while (1) {
			char user_choice[MSG_SIZE];

			if (show_menu)
				print_menu();
			else
				show_menu = 1;
			printf("What would you like to do?: ");
			scanf("%s", user_choice);
			printf("\n");

			if (!strcmp(user_choice, "logout")) {
				log_out();
				break;
			}
			if (!strcmp(user_choice, "exit") || !strcmp(user_choice, "x")) {
				log_out();
				exit(0);
			}
			else if (!strcmp(user_choice, "users")) {
				show_users();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "groups")) {
				show_groups();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "join")) {
				add_to_group();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "leave")) {
				remove_from_group();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "msg")) {
				send_msg_to_user();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "msggroup")) {
				send_msg_to_group();
				show_menu = 0;
			}
			else if (!strcmp(user_choice, "msgview")){
				view_msgs();
				show_menu = 0;
			}
			else {
				printf("\n### Wrong command, try again! ###\n");
			}
		}
	}
}