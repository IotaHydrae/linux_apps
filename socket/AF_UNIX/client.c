#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/demo.sock"

int main(void)
{
    int sockfd;

    struct sockaddr_un addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sockfd,
                (struct sockaddr *)&addr,
                sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    char buf[128];

    strcpy(buf, "Hello Server");

    send(sockfd, buf, strlen(buf) + 1, 0);

    recv(sockfd, buf, sizeof(buf), 0);

    printf("Receive: %s\n", buf);

    close(sockfd);

    return 0;
}
