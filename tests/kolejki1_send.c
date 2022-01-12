#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSGSZ 12

typedef struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
} message_buf;

int main() 
{
	key_t key = 2137;
	message_buf mb;
	size_t buf_length;

	int id = msgget(key, IPC_CREAT | 0666);

	mb.mtype = 1;
	strcpy(mb.mtext, "MESSAGE");

	buf_length = strlen(mb.mtext) + 1;

	msgsnd(id, &mb, buf_length, IPC_NOWAIT);
    printf("Sent message: %s\n", mb.mtext);

	exit(0);
}