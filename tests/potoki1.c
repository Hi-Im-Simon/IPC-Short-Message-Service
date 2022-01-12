#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	char tab[6] = "Hallo";
    char outtab[6];

    int pdesk[2];
    pipe(pdesk);
    int a = fork();

    if (a > 0) {
        read(pdesk[0], outtab, 6);
        printf("%s\n", outtab);
    }
    else {
        write(pdesk[1], tab, 6);
    }

	return 0;
}
