#ifndef MESSAGES_H
#define MESSAGES_H


#define DEF_SIZE 128
#define NAME_SIZE 128
#define MSG_SIZE 128

typedef struct buf_message {
	long mtype;
	char mtext[MSG_SIZE];
} buf_message;

typedef struct buf {
	long mtype;
} buf;

#endif