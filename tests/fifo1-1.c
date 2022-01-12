#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char tab[59] = "ps –ef | tr –s ‘ ‘  :| cut –d: -f1 |sort| uniq –c |sort –n";

    char * queue = "/tmp/aa";

    mkfifo(queue, 0666);
    int fd = open(queue, O_WRONLY);

    write(fd, tab, strlen(tab)+1);
    close(fd);
	return 0;
}
