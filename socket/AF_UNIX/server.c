#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/demo.sock"

int main(void)
{
    int listenfd;
    int connfd;

    struct sockaddr_un addr;

    unlink(SOCKET_PATH);

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (listenfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(listenfd,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    listen(listenfd, 5);

    printf("Server waiting...\n");

    connfd = accept(listenfd, NULL, NULL);

    char buf[128];

    recv(connfd, buf, sizeof(buf), 0);

    printf("Receive: %s\n", buf);

    strcpy(buf, "Hello Client");

    send(connfd, buf, strlen(buf) + 1, 0);

    close(connfd);
    close(listenfd);

    unlink(SOCKET_PATH);

    return 0;
}
