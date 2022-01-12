#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    	int f = open(argv[1], O_RDWR);
	char c;
	int isasc = 1;
    	while(read(f, &c, 1)) {
		if (!isascii(c))
			isasc = 1;
	}
    	close(f);
	if (isasc)
		printf("yes");
	else
		printf("no");
}
