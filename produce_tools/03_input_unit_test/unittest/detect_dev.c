/********************************************************************
 ____              _                 
| __ )  ___  _ __ | |__   ___  _ __  
|  _ \ / _ \| '_ \| '_ \ / _ \| '_ \ 
| |_) | (_) | | | | |_) | (_) | | | |
|____/ \___/|_| |_|_.__/ \___/|_| |_|
                                     
                                     
* @file    detect_dev.c                                                        
* @Author  huazheng                                                        
* @Date    2021-4-7                                                        
* @brief   another udp client                                                     
********************************************************************/

/**
 * client:
 *      (socket)open a socket
 *      (sendto receivefrom)send and receive data
 *      (close)close socket
 */


#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_PORT 9989
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)
#define SERVER_ADDRESS "127.0.0.1"

int main(int argc, char **argv)
{
    int len=0;
    int client_socket;
    struct sockaddr_in server_addr;

    if(argc != 3){
        printf("Usage: %s <server ip> <msg>\n", argv[0]);
        return -1;
    }

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_socket < 0 ){
        handle_error("socket error");
    }
    server_addr.sin_family = AF_INET;
    inet_aton(SERVER_ADDRESS, &server_addr.sin_addr);
    server_addr.sin_port = htons(SERVER_PORT);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));


    len=sendto(client_socket, argv[2], strlen(argv[2]), 0,
    (struct sockaddr *)&server_addr,
    sizeof(struct sockaddr));

    return 0;
}