#ifndef MESSAGES_H
#define MESSAGES_H


#define DEF_SIZE 128
#define NAME_SIZE 128
#define MSG_SIZE 256

#define KEY 2137

typedef struct msg_test {
	long type;
} msg_test;

typedef struct msg_txt {
	long type;
	char text[MSG_SIZE];
} msg_txt;

// set third argument as 4 (C int size)
typedef struct msg_int {
	long type;
	int value;
} msg_int;


#endif