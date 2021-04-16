#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <input_manager.h>

#define SERVER_PORT 9989
#define handle_error(msg)   \
		do						\
		{						\
			perror(msg);		\
			return -1; \
		} while (0)
#define BUFSIZE 256

/**
 * server:
 *		(socket)open a socket
 *		(bind)name the socket
 *		(sendto receivefrom)send and receive data
 *		(close)close socket
 */

/*
	struct sockaddr_in{
	unsigned short          sin_family;     
	unsigned short int      sin_port;
	struct in_addr          sin_addr;
	unsigned char           sin_zero[8];
}
*/


static int ret = 0;
static int len = 0;
static int server_socket;
static unsigned int client_addr_size;
static char msg_buf[BUFSIZE] = {0};
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;


int NetworkDeviceInit(void)
{
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ret < 0)
	{
		handle_error("socket error");
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	ret = bind(server_socket,
			   (struct sockaddr *)&server_addr,
			   sizeof(server_addr));
	if (ret < 0)
	{
		handle_error("bind error");
	}

	
    return 0;
}

int NetworkDeviceExit(void)
{
	close(server_socket);
    return 0;
}

int NetworkGetInputEvent(pInputEvent event_in)
{
	struct timeval tv;
    event_in->iType = INPUT_TYPE_NET;
    client_addr_size = sizeof(server_addr);
    len = recvfrom(server_socket,
                   msg_buf,
                   BUFSIZE,
                   0,
                   (struct sockaddr *)&client_addr,
                   &client_addr_size);

    if (len >= 0)
    {
        printf("Get message from client@%s: %s\n",
               inet_ntoa(client_addr.sin_addr),
               msg_buf);
		strncpy(event_in->str, msg_buf, 999);
		gettimeofday(&tv, NULL);


		event_in->tv = tv;
    }
	else
	{
		return -1;
	}
    return 0;
}

static InputDevice g_tNetworkInputDevice = {
    .cName = "Network",
    .DeviceInit = NetworkDeviceInit,
    .DeviceExit = NetworkDeviceExit,
    .GetInputEvent = NetworkGetInputEvent,
};
	
void RegisterNetworkInputDevice(void)
{
	ResgisterInputDevice(&g_tNetworkInputDevice);
}


#if 0
int main(int argc, const char *argv[])
{
    InputEvent event;
    int ret;
    g_tNetworkInputDevice.DeviceInit();
    while(1) {
        ret = g_tNetworkInputDevice.GetInputEvent(&event);
        if(ret) {
            printf("error on GetInputEvent\n");
            return -1;
        } else {
			printf("time        :%d\n", (int)event.tv.tv_sec);
            printf("Type        :%d\n", event.iType);
            printf("str         :%s\n\n\n", event.str);
        }
    }
    return 0;
}
#endif

