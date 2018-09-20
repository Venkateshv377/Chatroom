#include "header.h"

int sock_fd;
void* msgsend(void *sock_id)
{
    //    printf("Thread: sending\n");
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

    unsigned int activity;
    fd_set readfds;

    while(1){
        activity = recv(sockid, (struct sockaddr *)&msg, sizeof(msg), 0);
        if (errno == -1)
        {
            printf("Connection is closing...\n");
            close(sockid);
            return (int*)0;
        } else if (activity) {
            printf("%s\n", msg);
            memset(msg, '\0', strlen(msg));
        }
    }
}

void sock_close_handler(int sig)
{
    close(sock_fd);
    printf("Closing connection...\n");
    exit(0);
}

int main()
{
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
    {
        printf("Error: In connecting with server\n");
        return 0;
    } else
        printf("Connection is Successful\n");
#if 1
    printf("Welcome to chat room: sockfd: %d\n", sock_fd);
OPTION:
    printf("Username: ");
    scanf("%s", details.username);
    printf("password: ");
    scanf("%s", details.password);
    printf("1.Registration\t2.Login\n");
    scanf("%d", &details.key);
    //    printf("name: %s\tpass: %s\tkey: %d\n", details.username, details.password, details.key);

    int ret = send(sock_fd, (struct sockaddr*)&details, sizeof(details), 0); 
    printf("ret: %d\n", ret);

    recv(sock_fd, (struct sockaddr*)&details, 40, 0);
    if (details.ack == '0'){
        printf("User name is not existed, kindly register\n");
        goto OPTION;
    }else if (details.ack == '1'){
        printf("You have registerd successfully, kindly login\n");
        goto OPTION;
    }else if (details.ack == '2')
        printf("You have logged in successfully\n");
    else 
    {
        printf("Please choose the correct option\n");
        goto OPTION;
    }
#endif
    signal(SIGINT, sock_close_handler);
    int i, client_count, login_clients, userid;
    client array_clients[SERVER_LENGTH];

    recv(sock_fd, (struct sockaddr *)&client_count, sizeof(client_count), 0);
    printf("client_count: %d\n", client_count);
    recv(sock_fd, (struct sockaddr *)&array_clients, sizeof(array_clients), 0);
    printf("Available Clients...\n");
    for (i = 0; i < client_count; i++)
    {
        ret = strcmp(details.username, array_clients[i].username);
        printf("strcmp ret: %d\n", ret);
        if (ret){
            printf("%d: username: %s\t", i+1, array_clients[i].username);
            printf("desc_number: %d\n", array_clients[i].desc_no);
        }
#if 0
        else {
            printf("%d: username: %s\t", i+1, array_clients[i].username);
            printf("desc_number: %d\n", array_clients[i].desc_no);
        }
#endif
    }
    printf("Choose username with whom you want to communicate\n");
    scanf("%d", &userid);
    ret = send(sock_fd, (struct sockaddr*)&userid, sizeof(userid), 0); 
    printf("Sending requested client sock id: %d\tret: %d\t", userid, ret);

    pthread_create(&sendID, NULL, &msgsend, &sock_fd);
    pthread_create(&recvID, NULL, &msgrecv, &sock_fd);
    pthread_join(sendID, NULL);
    pthread_join(recvID, NULL);
    return 0;
}
