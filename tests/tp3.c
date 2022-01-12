#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	printf("Poczatek\n\n");
    execlp("ls", "ls", NULL);
    printf("\nKoniec\n");

	return 0;
}
