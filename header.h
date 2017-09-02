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

#pragma pack(1)

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6330
#define PATH "/home/venkatesh/Chatroom/Userdetails.txt"

typedef struct node
{
	int key;
	char username[20];
	char password[10];
	char ack;
}details_t;
