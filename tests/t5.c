#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    	int f = open(argv[1], O_RDWR);
	char c;
	char f_tab[1000];
	int i = 0;
    	while(read(f, &c, 1)) {
		f_tab[i] = c;
		i++;
	}
	i -= 2;
	while (i >= 0) {
		printf("%c", f_tab[i]);
		i--;
	}
    	close(f);
}
