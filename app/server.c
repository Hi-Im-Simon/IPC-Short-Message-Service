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


int id;


struct User {
	char login[NAME_SIZE];
	int group_ids[DEF_SIZE];
	int group_count;
	int online;
};

struct Group {
	char name[NAME_SIZE];
	int user_ids[DEF_SIZE];
	int user_count;
};


struct User users[DEF_SIZE];
struct Group groups[DEF_SIZE];
int users_count = 0;
int groups_count = 0;


void close_server() {
	printf("\nClosing server...\n");
	msgctl(id, IPC_RMID, 0);
	exit(0);
}

long await_msg_type() {
	msg_test mb;

	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	msgsnd(id, &mb, 0, IPC_NOWAIT);

	return mb.type;
}

void user_login() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 1;

	msgrcv(id, &mb, MSG_SIZE, 0, 0);

	int found = 0;
	int taken = 0;
	for (int i=0; i<=DEF_SIZE; i++) {
		if (!strcmp(users[i].login, mb.text)) {
			found = 1;
			if (users[i].online == 1)
				taken = 1;
			else 
				users[i].online = 1;
		}
	}

	if (taken) {
		mb_back.value = 0;
		printf("Server: attempt in logging into a taken user - %s\n", mb.text);
	}
	else if (found) {
		mb_back.value = 1;
		printf("Server: old user logging in - %s\n", mb.text);
	}
	else {
		mb_back.value = 2;
		strcpy(users[users_count].login, mb.text);
		users[users_count].online = 1;
		users_count++;
		printf("Server: new user logging in - %s\n", mb.text);
	}

	mb_back.type = 1;
	msgsnd(id, &mb_back, MSG_SIZE, IPC_NOWAIT);
}

void user_logout() {
	msg_txt mb;
	msg_test mb_back;
	mb_back.type = 2;

	msgrcv(id, &mb, MSG_SIZE, 0, 0);

	for (int i=0; i<=DEF_SIZE; i++) {
		if (!strcmp(users[i].login, mb.text)) {
			users[i].online = 0;
		}
	}
	printf("Server: user logging out - %s\n", mb.text);

	msgsnd(id, &mb_back, MSG_SIZE, IPC_NOWAIT);
}

void list_users() {
	msg_test mb;
	msg_int mb_back;
	mb_back.type = 3;

	msgrcv(id, &mb, 0, 0, 0);

	int users_online_count = 0;

	for (int i=0; i<=DEF_SIZE; i++) {
		if (users[i].online == 1) {
			users_online_count++;
		}
	}
	mb_back.value = users_online_count;

	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	msg_txt mb_back2;
	mb_back2.type = 3;

	for (int i=0; i<=DEF_SIZE; i++) {
		if (users[i].online == 1) {
			strcpy(mb_back2.text, users[i].login);
			msgrcv(id, &mb, 0, 0, 0);
			msgsnd(id, &mb_back2, MSG_SIZE, IPC_NOWAIT);
		}
	}
}

void list_groups() {
	msg_test mb;
	msg_int mb_back;
	mb_back.type = 4;

	msgrcv(id, &mb, 0, 0, 0);

	mb_back.value = groups_count;

	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	msg_txt mb_back2;
	mb_back2.type = 4;

	for (int i=0; i<groups_count; i++) {
		mb_back.value = groups[i].user_count;
		strcpy(mb_back2.text, groups[i].name);

		msgrcv(id, &mb, 0, 0, 0);
		msgsnd(id, &mb_back, 4, IPC_NOWAIT);
		msgrcv(id, &mb, 0, 0, 0);
		msgsnd(id, &mb_back2, MSG_SIZE, IPC_NOWAIT);

		for (int j=0; j<groups[i].user_count; j++) {
			strcpy(mb_back2.text, users[groups[i].user_ids[j]].login);

			msgrcv(id, &mb, 0, 0, 0);
			msgsnd(id, &mb_back2, MSG_SIZE, IPC_NOWAIT);
		}
			// strcpy(mb_back2.text, groups[i].name);
	}
}

void add_user_to_group() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 5;

	char group_name[MSG_SIZE];
	char user_name[MSG_SIZE];

	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(group_name, mb.text);
	msgsnd(id, &mb_back, 0, IPC_NOWAIT);
	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(user_name, mb.text);

	for (int i=0; i<groups_count; i++) {
		if (!strcmp(group_name, groups[i].name)) {
			for (int j=0; j<groups[i].user_count; j++) {
				if (!strcmp(user_name, users[groups[i].user_ids[j]].login)) {
					mb_back.value = 2;
					msgsnd(id, &mb_back, 4, IPC_NOWAIT);

					printf("Server: user '%s' tried to join group '%s', which they already belong to\n", user_name, group_name);
					return;
				}
			}

			// finally if correct group is chosen:
			for (int k=0; k<=DEF_SIZE; k++) {
				if (!strcmp(users[k].login, user_name)) {
					groups[i].user_ids[groups[i].user_count] = k;
					groups[i].user_count++;
					users[k].group_ids[users[k].group_count] = i;
					users[k].group_count++;

					printf("%d, %d, %s, %s\n", groups[i].user_count, users[k].group_count, groups[i].name, users[k].login);

					break;
				}
			}

			mb_back.value = 1;
			msgsnd(id, &mb_back, 4, IPC_NOWAIT);

			printf("Server: adding user '%s' to group '%s'\n", user_name, group_name);
			return;
		}
	}
	mb_back.value = 0;
	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	printf("Server: user '%s' tried to join group '%s', which doesn't exist\n", user_name, group_name);
}


int main(int argc, char* argv[]) {
	id = msgget(KEY, IPC_CREAT | 0666);
	signal(SIGINT, close_server);

	// defining initial groups
	strcpy(groups[0].name, "G1");
	strcpy(groups[1].name, "G2");
	strcpy(groups[2].name, "G3");
	groups_count += 3;

	// groups[0].user_ids[0] = 0;
	// groups[0].user_count++;
	// groups[2].user_ids[0] = 0;
	// groups[2].user_count++;

	while (1) {
		long msg_type = await_msg_type();
		printf("New message type is: %ld\n", msg_type);
		
		switch (msg_type) {
			// user logging in
			case 1:
				user_login();
				break;
			case 2:
				user_logout();
				break;
			case 3:
				list_users();
				break;
			case 4:
				list_groups();
				break;
			case 5:
				add_user_to_group();
				break;

		}
	}
}