#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int a;
	for (int i=0; i<2; i++) {
		a = fork();
		if (a > 0) {
			printf("Parent ID: %d\n", getpid());
		}
		else if (a == 0) {
			printf("Child ID: %d\n", getpid());
		}
	}
	return 0;
}
// cos nie tak jest