#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include <poll.h>

#pragma pack(1)

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define PATH "/home/venkatesh/Chatroom/Userdetails.txt"
#define SERVER_LENGTH   5

typedef struct 
{
	int key;
	char username[20];
	char password[10];
	char ack;
} details_t;

typedef struct
{
    char username[20];
    int flag;
    int desc_no;
} client;

typedef enum {
	REGISTER,
	LOGIN,
	LOGIN_SUCCESSFUL,
	INVALID_OPT,
} client_status;
