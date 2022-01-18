#ifndef MESSAGES_H
#define MESSAGES_H


#define DEF_SIZE 128
#define NAME_SIZE 128
#define MSG_SIZE 256

typedef struct buf_message {
	long type;
	char text[MSG_SIZE];
} buf_message;

typedef struct buf_int {
	long type;
	int value;
} buf_int;

typedef struct buf {
	long type;
} buf;

#endif