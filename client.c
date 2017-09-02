#include "header.h"

void* msgsend(void *sock_id)
{
	printf("Thread: sending\n");
	int *id = (int*)sock_id;
	int sockid = *id;
	char msg[100];
	while(1){
		scanf("%s", msg);
		send(sockid, (struct sockaddr *)&msg, strlen(msg), 0);
		memset(msg, '\0', sizeof(msg));
	}
}
void* msgrecv(void* sock_id)
{
	printf("Thread: receiving\n");
	int *sock = (int*)sock_id;
	int sockid = *sock;
	char msg[100];

	int activity;
	fd_set readfds;

	while(1){
		recv(sockid, (struct sockaddr *)&msg, sizeof(msg), 0);
		printf("%s\n", msg);
		memset(msg, '\0', strlen(msg));
	}
}

int main()
{
	int sock_fd, data_fd, c_size, len;
	struct sockaddr_in serv_addr;
	details_t details = {'\0'};

	pthread_t sendID, recvID;

	/* Create a client socket*/
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\nError\n");
		return 1;
	}

	//Populate it with server details
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);   //Takes the string, and gives out as a 4-byte int
	serv_addr.sin_port = htons(SERVER_PORT);

	//Connection with server
	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
		printf("Error: In connecting with server\n");
	else
		printf("Connection is Successful\n");

	printf("Welcome to chat room\n");
	printf("Username: ");
	gets(details.username);
	printf("password: ");
	gets(details.password);
	printf("1.Registration\t2.Login\n");
	scanf("%d", &details.key);

	send(sock_fd, (struct sockaddr*)&details, sizeof(details), 0); 

	recv(sock_fd, (struct sockaddr*)&details, 40, 0);
	if (details.ack == '0')
		printf("User name is not existed\n");
	else if (details.ack == '1')
		printf("You have registerd successfully\n");
	else if (details.ack == '2')
		printf("You have logged in successfully\n");
	else if (details.ack == '3')
		printf("Please choose the correct option\n");

	pthread_create(&sendID, NULL, &msgsend, &sock_fd);
	pthread_create(&recvID, NULL, &msgrecv, &sock_fd);
	pthread_join(sendID, NULL);
	pthread_join(recvID, NULL);
	return 0;
}
