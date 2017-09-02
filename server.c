#include "header.h"

struct sockaddr_in serv_addr;
details_t details;
int fp, pid, status;
char buffer[40], *token, *temp, buff[10];
FILE *fs;
int ack;

void* msg_recv(void* descriptor)
{
	char msg_buff[100];
	int *data = (int*)descriptor;
	int data_sock_fd = *data;
	int ret;

	while(1){
		ret=recv(data_sock_fd, (struct sockaddr *)&msg_buff, sizeof(msg_buff), 0);
		printf("%s\n", msg_buff);
		memset(msg_buff, '\0', strlen(msg_buff));
		if (errno == -1){
			printf("Connection is closing...\n");
			close(data_sock_fd);
			return;
		}
	}
}
void* msg_send(void* descriptor)
{
	char msg_buff[100];
	int *data = (int*)descriptor;
	int data_sock_fd = *data;
	while(1){
		scanf("%s", msg_buff);
		send(data_sock_fd, (struct sockaddr *)&msg_buff, strlen(msg_buff), 0);
		memset(msg_buff, '\0', strlen(msg_buff));
	}
}


void* thread_handler(void* file_descriptor)
{
	int flag = 0;
	int *data = (int*)file_descriptor;
	int data_sock_fd = *data;
	pthread_t thread_ID1, thread_ID2;

	//	printf("data_sock_fd in thread: %d\n", data_sock_fd);
	if (recv(data_sock_fd, (struct sockaddr *)&details, sizeof(details), 0))
		printf("Username is %s\nPassword is %s\n Key:   %d\n", 
				details.username, details.password, details.key);

	if (details.key == 2)
	{
		fs = fopen(PATH, "r");
		if (fs == NULL)
		{
			printf("File not found\n");
			exit(1);
		}
		bzero(buffer, 30);
		bzero(buff, 10);
		while (fgets(buffer, 30, fs))
		{
			token = strtok(buffer, ":");
			temp = strdup(token);
			token = strtok(NULL, ":");
			if ((!strcmp(token, details.password)) && (!strcmp(temp, details.username)))
			{
				details.ack = '2'; 
				send(data_sock_fd, (struct sockaddr *)&details, 40,0);
				printf("You have logged in successfully\n");
				goto CHAT;
			}
		}
		details.ack = '0';
		printf("Username is not existed, please register\n");
	}
	else if (details.key == 1)
	{
		write(fp, details.username, strlen(details.username));
		write(fp, ":", strlen(":"));
		write(fp, details.password, strlen(details.password));
		write(fp, ":\n", strlen(":\n"));
		printf("You are registerd successfully, please login\n");
		details.ack = '1';
		send(data_sock_fd, (struct sockaddr *)&details, sizeof(details),0);

	}
	else 
	{
		details.ack = '3';
		printf("Please select the valid option\n");
		send(data_sock_fd, (struct sockaddr *)&details, sizeof(details),0);
	}
CHAT:	printf("CHAT\n");
	send(data_sock_fd, (struct sockaddr *)&details, sizeof(details),0);
	pthread_create(&thread_ID1, NULL, &msg_recv, &data_sock_fd);
	pthread_create(&thread_ID2, NULL, &msg_send, &data_sock_fd);
	pthread_join(thread_ID1, NULL);
	pthread_join(thread_ID2, NULL);
}


int main()
{
	int sock_fd, data_sock_fd, SERVER_LENGTH;
	pthread_t threadID;

	system("clear");
	printf("Server is waiting\n");
	//create a socket
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  //Control connection
	{
		printf("Error: In socket creation\n");
		exit(1);
	}

	//bind it to a particular xm

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);
	bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	//listen
	listen(sock_fd, SERVER_LENGTH);

	//Opening the file in read, write and append mode
	mknod(PATH, 0777, 0);
	fp = open(PATH, O_RDWR | O_APPEND);

	while(1)
	{
		//wait & accept, if available 
		if ((data_sock_fd = accept(sock_fd, (struct sockaddr *)NULL, NULL)) == -1)
			printf("Error: In connecting with client\n");
		else
			printf("Connection is Successfull\n");

		printf("data_sock_fd: %d\n", data_sock_fd);

		pthread_create(&threadID, NULL, &thread_handler, &data_sock_fd);
		//pthread_join(threadID, NULL);
	}
	return 0;
}
