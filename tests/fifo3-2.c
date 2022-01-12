#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

// CLIENT
int main(int argc, char *argv[]) {
    char my_queue[8] = "/tmp/q1";
    char * queue = "/tmp/fifo";

    mkfifo(queue, 0666);

    int fd = open(queue, O_WRONLY);
    write(fd, my_queue, strlen(my_queue)+1);
    close(fd);

    char output[13];

    mkfifo(my_queue, 0666);

    fd = open(my_queue, O_RDONLY);
    read(fd, output, strlen(output)+1);
    printf("%s\n", output);
    close(fd);
	return 0;
}
