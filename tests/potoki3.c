#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	char tab1[6] = "Hallo";
    char tab2[6] = "user!";
    char outtab[6];

    int pdesk[2];
    pipe(pdesk);
    int a = fork();
    int b = fork();
    int c = fork();
    int i = 0;
    
    if (a == 0) {
        read(pdesk[0], outtab, 6);
        printf("%s\n", outtab);
        close(1);
    }
    else if (b == 0) {
        // cos tu nie tak
        waitpid(a);
        write(pdesk[1], tab1, 6);
    }
    else if (c == 0) {
        waitpid(a);
        write(pdesk[1], tab2, 6);
    }
	return 0;
}
