#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

static void *watch_dir(void *arg)
{
    int fd, wd;
    char *path = (char *)arg;
    struct stat sb;

    fd = inotify_init();
    char buffer[BUF_LEN];
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring directory %s...\n", path);

    while (1) {
        ssize_t len = read(fd, buffer, BUF_LEN);
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        char *ptr = buffer;
        while (ptr < buffer + len) {
            struct inotify_event *event = (struct inotify_event *)ptr;

            if (event->mask & IN_CREATE && event->mask)
                printf("File created: %s\n", event->name);
            else if (event->mask & IN_DELETE)
                printf("File deleted: %s\n", event->name);
            else if (event->mask & IN_MODIFY)
                printf("File modified: %s\n", event->name);

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [directory]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, watch_dir, argv[1]);

    while (1) { sleep(1); }

    exit(EXIT_SUCCESS);
}

