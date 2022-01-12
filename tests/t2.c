#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 100

int main(int argc, char *argv[])
{
    int f = open(argv[1], O_RDWR);

    char c;
    int i = 0;
    char cs[SIZE];
    while (read(f, &c, 1)) {
        if (c == '\n') {
            lseek(f, -i - 1, SEEK_CUR);
            for (i=i-1; i >= 0; i--) {
                write(f, &cs[i], 1);
            }
            write(f, &c, 1);
            i = 0;
        }
        else {
            cs[i] = c;
            i++;
        }
    }

    close(f);
}
