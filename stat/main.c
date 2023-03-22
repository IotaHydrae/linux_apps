#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [path]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(sb.st_mode))
        printf("%s is a regular file\n", argv[1]);
    else if (S_ISDIR(sb.st_mode))
        printf("%s is a directory\n", argv[1]);
    else
        printf("%s is neither a regular file nor a directory\n", argv[1]);

    exit(EXIT_SUCCESS);
}
