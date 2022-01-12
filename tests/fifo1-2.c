#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char outtab[59];

    char * queue = "/tmp/aa";

    mkfifo(queue, 0666);
    int fd = open(queue, O_RDONLY);

    read(fd, outtab, strlen(outtab)+1);
    system(outtab);
    close(fd);
	return 0;
}
