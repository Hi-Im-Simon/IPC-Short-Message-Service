#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGSZ 128

typedef struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
} message_buf;

int main()
{
	key_t key = 2137;
	message_buf mb;

	int id = msgget(key, 0666);

	if (msgrcv(id, &mb, MSGSZ, 1, 0) < 0) {
		perror("msgrcv");
		exit(1);
	}

	printf("Received message: %s\n", mb.mtext);
	exit(0);
}