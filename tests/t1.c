#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int f1 = open(argv[1], O_RDONLY);
    int f2 = open(argv[2], O_WRONLY|O_CREAT, 0700);
    
    char c;
    while(read(f1, &c, 1)) {
        write(f2, &c, 1);
    }

    close(f1);
    close(f2);
}
