#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	for (int fi=1; fi < argc; fi++) {
		int f = open(argv[fi], O_RDWR);
		char c;
		int i = 0;
		while (read(f, &c, 1)) {
			if (c != '\n')
				i++;
		}
		printf("Plik %d: %d\n", fi, i);
		close(f);
	}
}
