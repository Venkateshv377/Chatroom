#include "header.h"

struct sockaddr_in serv_addr;
details_t details;
int fp, pid;
char buffer[40], *token, *temp, *flag, buff[10];
FILE *fs;
int client_list[SERVER_LENGTH], i=0, id, client_count;

client array_clients[SERVER_LENGTH];

char string[9];

char *itoa(int val)
{
    int temp, i=0;
    while(val)
    {
        temp = val%10;
        string[i] = (char)(temp + 48);
        val = val / 10;
        i++;
    }
    return string;
}

void* msg_recv(void* descriptor)
{
    char msg_buff[100];
    int *data = (int*)descriptor;
    int data_sock_fd = *data;
    unsigned int ret;

    while(1){
        ret=recv(data_sock_fd, (struct sockaddr *)&msg_buff, sizeof(msg_buff), 0);
        if (errno == -1){
            printf("Connection is closing...\n");
            close(data_sock_fd);
            return (int*)0;
        }
        else if (ret) {
            printf("%s\n", msg_buff);
            memset(msg_buff, '\0', strlen(msg_buff));
        }
        else if (ret == 0) {
            getpeername(data_sock_fd, (struct sockaddr *)&serv_addr, NULL);
            printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(serv_addr.sin_addr) , ntohs(serv_addr.sin_port));
#if 1
            for (int i = 0; i < client_count; i++)
            {
                if (array_clients[i].desc_no == data_sock_fd)
                    array_clients[i].desc_no = 0;
                printf("username: %s\n", array_clients[i].username);
                printf("status: %d\n", array_clients[i].flag);
                printf("socket_id: %d\n", array_clients[i].desc_no);
            }
#endif
            close(data_sock_fd);
            return (int*)0;
        }
    }
}
void* msg_send(void* descriptor)
{
    char msg_buff[100];
    int *data = (int*)descriptor;
    unsigned int data_sock_fd = *data, ret;
    while(1){
        gets(msg_buff);
        ret = send(data_sock_fd, (struct sockaddr *)&msg_buff, strlen(msg_buff), 0);
        printf("Send: %d\t desc: %d\n", ret, data_sock_fd);
        if (ret == -1){
            perror("Error: \n");
            //        close(data_sock_fd);
            return (int*)0;
        } else if(ret > 0){
            memset(msg_buff, '\0', strlen(msg_buff));
            printf("Sent %d...Error: %d... File Desc: %d\n", ret, EACCES, data_sock_fd);
        }
    }
}


void* thread_handler(void* file_descriptor)
{
    int *data = (int*)file_descriptor;
    int data_sock_fd = *data;
    pthread_t thread_ID1, thread_ID2;

OPTION:

    if (recv(data_sock_fd, (struct sockaddr *)&details, sizeof(details), 0))
        printf("Username is %s\nPassword is %s\n Key:   %d\n", 
                details.username, details.password, details.key);

    if (details.key == 2)
    {
        fs = fopen(PATH, "a+");
        if (fs == NULL)
        {
            printf("File not found\n");
            exit(1);
        }
        bzero(buffer, 30);
        bzero(buff, 10);
        while (fgets(buffer, 30, fs))
        {
            printf("entering\n");
            token = strtok(buffer, ":");
            temp = strdup(token);
            token = strtok(NULL, ":");
            flag = strtok(NULL, ":");
            if ((!strcmp(token, details.password)) && (!strcmp(temp, details.username)))
            {
                strcpy(array_clients[client_count].username, temp);
                array_clients[client_count].flag = 1;
                array_clients[client_count].desc_no = atoi(flag);

                client_count++;
                details.ack = '2'; 
                send(data_sock_fd, (struct sockaddr *)&details, 40,0);
                printf("You have logged in successfully\n");
                printf("Available clients...\n");
                return (int *)LOGIN_SUCCESSFUL;
            }
        }
        details.ack = '0';
        printf("Username is not existed, please register\n");
        send(data_sock_fd, (struct sockaddr *)&details, 40,0);
        goto OPTION;
        return (int *)REGISTER;
    }
    else if (details.key == 1)
    {
        printf("Entering into registration\n");
        write(fp, details.username, strlen(details.username));
        write(fp, ":", strlen(":"));
        write(fp, details.password, strlen(details.password));
        write(fp, ":", strlen(":"));
        char *itoa_sock = itoa(data_sock_fd);
        write(fp, itoa_sock, strlen(itoa_sock));
        write(fp, ":\n", strlen(":\n"));
        printf("You are registerd successfully, please login\n");
        details.ack = '1';
        send(data_sock_fd, (struct sockaddr *)&details, sizeof(details),0);
        goto OPTION;
        return (int *)LOGIN;

    }
    else 
    {
        details.ack = '3';
        printf("Please select the valid option\n");
        send(data_sock_fd, (struct sockaddr *)&details, sizeof(details),0);
        goto OPTION;
        return (int *)INVALID_OPT;
    }
}

int main()
{
    int sock_fd, data_sock_fd;
    void *status;
    pthread_t threadID, send_id, recv_id;

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
        {
            printf("Error: In connecting with client\n");
            exit(0);
        } else {
            printf("Connection is Successfull\n");
        }

        printf("sock_fd: %d\n", sock_fd);
        printf("data_sock_fd: %d\n", data_sock_fd);

        pthread_create(&threadID, NULL, &thread_handler, &data_sock_fd);
        pthread_join(threadID, &status);
        printf("Status of user: %d\n", (int)status);
#if 0
        for (int i = 0; i < client_count; i++)
        {
            printf("username: %s\n", array_clients[i].username);
            printf("status: %d\n", array_clients[i].flag);
            printf("socket_id: %d\n", array_clients[i].desc_no);
        }
#endif
        send(data_sock_fd, (struct sockaddr *)&client_count, sizeof(client_count),0);
        send(data_sock_fd, (struct sockaddr *)&array_clients, sizeof(array_clients),0);
        uint32_t ret, req_cl_sock_id;
        ret = recv(data_sock_fd, (struct sockaddr *)&req_cl_sock_id, sizeof(req_cl_sock_id), 0);
        printf("ret: %d\t req_cl_sock_id: %d\n", ret, req_cl_sock_id);

        for (int i = 0; i < client_count; i++)
        {
            printf("username: %s\n", array_clients[i].username);
            printf("status: %d\n", array_clients[i].flag);
            printf("socket_id: %d\n", array_clients[i].desc_no);
            if (i == req_cl_sock_id)
                data_sock_fd = array_clients[i].desc_no;
        }
        printf("data_sock_fd: %d\n", data_sock_fd);
#if 1
        id = fork();
        if (id == 0){
            send_id = recv_id = data_sock_fd;
            pthread_create(&send_id, NULL, &msg_recv, &data_sock_fd);
            pthread_create(&recv_id, NULL, &msg_send, &data_sock_fd);
            pthread_join(send_id, NULL);
            pthread_join(recv_id, NULL);
        }
#endif
    }
    return 0;
}
