#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// SERVER
int main(int argc, char *argv[]) {
	char client_queue[8];
    char * queue = "/tmp/fifo";

    mkfifo(queue, 0666);

    int fd = open(queue, O_RDONLY);
    read(fd, client_queue, strlen(client_queue)+1);
    printf("%s\n", client_queue);
    close(fd);

    char input[13] = "niechcemisie";

    mkfifo(client_queue, 0666);

    fd = open(client_queue, O_WRONLY);
    write(fd, input, strlen(input)+1);
    close(fd);
	return 0;
}
