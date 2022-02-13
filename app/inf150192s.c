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


struct User {
	char login[NAME_SIZE];
	int group_ids[DEF_SIZE];
	int group_count;
	int online;
	char msgs[DEF_SIZE][MSG_SIZE];
	int msg_count;
};

struct Group {
	char name[NAME_SIZE];
	int user_ids[DEF_SIZE];
	int user_count;
	char msgs[DEF_SIZE][MSG_SIZE];
	int msg_count;
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

void remove_user_from_group() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 6;

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
					// REMOVE USER user_name, user_ids[j]
					// FROM GROUP group_name, groups[i]
					for (int pp=j; pp<groups[i].user_count-1; pp++) {
						// move all users to left
						groups[i].user_ids[pp] = groups[i].user_ids[pp+1];
					}
					groups[i].user_count--;

					for (int gg=i; gg<users[groups[i].user_ids[j]].group_count-1; gg--) {
						users[groups[i].user_ids[j]].group_ids[gg] = users[groups[i].user_ids[j]].group_ids[gg+1];
					}
					users[groups[i].user_ids[j]].group_count--;

					mb_back.value = 1;
					msgsnd(id, &mb_back, 4, IPC_NOWAIT);
					printf("Server: removing user '%s' from group '%s'\n", user_name, group_name);
					return;
				}
			}

			mb_back.value = 2;
			msgsnd(id, &mb_back, 4, IPC_NOWAIT);

			printf("Server: user '%s' tried to leave group '%s', which they don't belong to\n", user_name, group_name);
			return;
		}
	}
	mb_back.value = 0;
	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	printf("Server: user '%s' tried to leave group '%s', which doesn't exist\n", user_name, group_name);
}

void msg_user_user() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 7;

	char receiver_name[MSG_SIZE];
	char sender_name[MSG_SIZE];
	char message[MSG_SIZE];
	char final_message[MSG_SIZE];

	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(receiver_name, mb.text);
	msgsnd(id, &mb_back, 0, IPC_NOWAIT);
	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(sender_name, mb.text);
	msgsnd(id, &mb_back, 0, IPC_NOWAIT);
	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(message, mb.text);

	for (int i=0; i<users_count; i++) {
		if (!strcmp(receiver_name, users[i].login)) {
			strcpy(final_message, sender_name);
			strcat(final_message, ": ");
			strcat(final_message, message);

			strcpy(users[i].msgs[users[i].msg_count], final_message);
			users[i].msg_count++;

			mb_back.value = 1;
			msgsnd(id, &mb_back, 4, IPC_NOWAIT);
			
			printf("Server: user '%s' sent a message to user '%s'\n", sender_name, receiver_name);
			return;
		}
	}

	mb_back.value = 0;
	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	printf("Server: user '%s' tried to send a message to user '%s', which doesn't exist\n", sender_name, receiver_name);
}

void msg_user_group() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 8;

	char group_name[MSG_SIZE];
	char sender_name[MSG_SIZE];
	char message[MSG_SIZE];
	char final_message[MSG_SIZE];

	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(group_name, mb.text);
	msgsnd(id, &mb_back, 0, IPC_NOWAIT);
	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(sender_name, mb.text);
	msgsnd(id, &mb_back, 0, IPC_NOWAIT);
	msgrcv(id, &mb, MSG_SIZE, 0, 0);
	strcpy(message, mb.text);

	for (int i=0; i<groups_count; i++) {
		if (!strcmp(group_name, groups[i].name)) {
			// find user id
			int user_id;
			for (int j=0; j<users_count; j++) {
				if (!strcmp(sender_name, users[j].login)) {
					user_id = j;
					break;
				}
			}

			// check if the user is in the chosen group
			int found = 0;
			for (int k=0; k<groups[i].user_count; k++) {
				if (groups[i].user_ids[k] == user_id) {
					found = 1;
					break;
				}
			}

			if (!found) {
				mb_back.value = 2;
				msgsnd(id, &mb_back, 4, IPC_NOWAIT);

				printf("Server: user '%s' tried to send a message to group '%s', which they don't belong to\n", sender_name, group_name);
				return;
			}

			strcpy(final_message, group_name);
			strcat(final_message, "] ");
			strcat(final_message, sender_name);
			strcat(final_message, ": ");
			strcat(final_message, message);

			strcpy(groups[i].msgs[groups[i].msg_count], final_message);
			groups[i].msg_count++;

			mb_back.value = 1;
			msgsnd(id, &mb_back, 4, IPC_NOWAIT);
			
			printf("Server: user '%s' sent a message to group '%s'\n", sender_name, group_name);
			return;
		}
	}

	mb_back.value = 0;
	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	printf("Server: user '%s' tried to send a message to group '%s', which doesn't exist\n", sender_name, group_name);
}

void list_msgs() {
	msg_txt mb;
	msg_int mb_back;
	mb_back.type = 9;
	int user_id;

	msgrcv(id, &mb, NAME_SIZE, 0, 0);

	for (int i=0; i<users_count; i++)
		if (!strcmp(mb.text, users[i].login))
			user_id = i;

	// send back how many msgs the user has in inbox (from users)
	mb_back.value = users[user_id].msg_count;

	msgsnd(id, &mb_back, 4, IPC_NOWAIT);
	msgrcv(id, &mb, NAME_SIZE, 0, 0);

	// send back how many msgs the user has in inbox (from groups)
	int group_msgs_count = 0;

	for (int gr=0; gr<groups_count; gr++) {
		for (int us=0; us<groups[gr].user_count; us++) {
			if (groups[gr].user_ids[us] == user_id) {
				group_msgs_count += groups[gr].msg_count;
			}
		}
	}

	mb_back.value = group_msgs_count;

	msgsnd(id, &mb_back, 4, IPC_NOWAIT);

	msg_txt mb_back2;
	mb_back2.type = 9;

	for (int i=0; i<users[user_id].msg_count; i++) {
		strcpy(mb_back2.text, users[user_id].msgs[i]);

		msgrcv(id, &mb, 0, 0, 0);
		msgsnd(id, &mb_back2, MSG_SIZE, IPC_NOWAIT);
	}

	for (int gr=0; gr<groups_count; gr++) {
		for (int us=0; us<groups[gr].user_count; us++) {
			if (groups[gr].user_ids[us] == user_id) {
				for (int ms=0; ms<groups[gr].msg_count; ms++) {
					strcpy(mb_back2.text, groups[gr].msgs[ms]);

					msgrcv(id, &mb, 0, 0, 0);
					msgsnd(id, &mb_back2, MSG_SIZE, IPC_NOWAIT);
				}
			}
		}
	}
}


int main(int argc, char* argv[]) {
	id = msgget(KEY, IPC_CREAT | 0666);
	signal(SIGINT, close_server);

	// defining initial groups
	strcpy(groups[0].name, "G1");
	strcpy(groups[1].name, "G2");
	strcpy(groups[2].name, "G3");
	groups_count += 3;

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
			case 6:
				remove_user_from_group();
				break;
			case 7:
				msg_user_user();
				break;
			case 8:
				msg_user_group();
				break;
			case 9:
				list_msgs();
				break;
		}
	}
}