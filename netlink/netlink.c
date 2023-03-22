#include <linux/netlink.h>
#include <poll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static void die(char *s)
{
    write(2, s, strlen(s));
    exit(1);
}

int main(int argc, char **argv)
{
    struct sockaddr_nl nladdr;
    struct pollfd pfd;

    char buf[512];

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = -1;

    pfd.events = POLLIN;

    pfd.fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (pfd.fd < 0)
        die("Not root\n");
    
    /* Listen to netlink socket */
    if(bind(pfd.fd, (void*)&nladdr, sizeof(nladdr)) < 0)
        die("Bind failed!\n");
    
    while(-1 != poll(&pfd, 1, -1)) {
        int i,len = recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
        if(len == -1)
            die("Recv failed!\n");

        /* Print the data to stdout */
        i = 0;
        while(i < len) {
            printf("%s\n", buf + i);
            i += strlen(buf + i) + 1;
        }
    }
    die("Poll failed!\n");

    /* Dear gcc: shut up */
    return 0;
}
