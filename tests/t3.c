#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    	int f = open(argv[1], O_RDWR);
	char c;
	int i = 0;
	int max_i = 0;
    	while(read(f, &c, 1)) {
		if (c == '\n') {
			if (i > max_i) {
				max_i = i;
			}
			i = 0;
		}
		else {
			i++;
		}
	}
	printf("%d\n", max_i);
    	close(f);
}
