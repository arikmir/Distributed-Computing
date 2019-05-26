#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define MYPORT 12345


#define BACKLOG 10

struct leaderBoard{
    char name[50];
    int gameTime;
    int gameWon;
    int gamePlayed;
};
void bsortDesc(struct leaderBoard list[80], int s)
{
    int i, j;
    struct leaderBoard temp;
    
    for (i = 0; i < s - 1; i++)
    {
        for (j = 0; j < (s - 1-i); j++)
        {
            if (list[j].gameTime < list[j + 1].gameTime)
            {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}
void displayLeaderboard(struct leaderBoard list[80], int s)
{
    int i;
    for (i = 0; i < s; i++)
    {
        printf("%s\t%d\t%d\t%d\n", list[i].name, list[i].gameTime, list[i].gameWon, list[i].gamePlayed);
    }
}
void inputIntoLeaderboard(struct leaderBoard *list[80], int s, char name[], int gameTime, int gameWon, int gamePlayed)
{
    strcpy(list[s]->name, "a");
    list[s]->gameTime = 232;
    list[s]->gameWon = 1;
    list[s]->gamePlayed = 3;
}

bool authenticateUser(int new_fd){
    bool authenticated = false;
    char username[100], password[100];
    char checkUser[100], checkPass[100];
    char getBoard[30];
    recv(new_fd , username , 2000 , 0);
    recv(new_fd , password , 2000 , 0);
    FILE *authentication;
    authentication = fopen("Authentication.txt","r");
    if(authentication == NULL)
    {
        printf("Error!");
        exit(1);
    }
    int c;
    while ((c = getc(authentication)) != EOF)
    {
        ungetc(c, authentication);
        fscanf(authentication,"%s", checkUser);
        fscanf(authentication,"%s", checkPass);
        puts(checkPass);
        puts(checkUser);
        if(strcmp(checkUser, username) == 0 && strcmp(checkPass, password) == 0){
            //puts("true");
            authenticated = true;
            break;
        }
        else{
            //puts("false");
            authenticated = false;
        }
    }
    fclose(authentication);
    if(authenticated == true)
        send(new_fd , "1" , 1 , 0);
    else
        send(new_fd , "0" , 1 , 0);
    recv(new_fd , getBoard , 2000 , 0);
    if(strcmp(getBoard, "showLeaderboard") == 0){
        send(new_fd , "1" , 1 , 0);
    }
    return authenticated;
}
void *connection_handler(void *socket_desc)
{
    char getBoard[30];
    int sock = *(int*)socket_desc;
    recv(sock , getBoard , 2000 , 0);
    if(strcmp(getBoard, "showLeaderboard") == 0){
        send(sock , "1" , 1 , 0);
    }
    free(socket_desc);
    
    return 0;
}


int main(int argc, char *argv[])
{
    int *new_sock;
    int definedPort = 0;
    int totalUserInLeaderboard = 0;
    if(argc > 1){
        definedPort = atoi(argv[1]);
    }
    int sockfd, new_fd;  /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    socklen_t sin_size;
    
    /* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    
    /* generate the end point */
    my_addr.sin_family = AF_INET;         /* host byte order */
    if(argc > 1)
        my_addr.sin_port = htons(definedPort);     /* short, network byte order */
    else
        my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    /* bzero(&(my_addr.sin_zero), 8);   ZJL*/     /* zero the rest of the struct */
    
    /* bind the socket to the end point */
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
        == -1) {
        perror("bind");
        exit(1);
    }
    
    /* start listnening */
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf("Server starts listnening at port %d ... \n", argc > 1 ? definedPort : MYPORT);
    
    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while(1) {  /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
                             &sin_size)) == -1) {
            perror("accept");
            continue;
        }
        if (listen(sockfd, BACKLOG) == -1) {
            perror("listen");
            exit(1);
        }
        
        printf("server starts listnening ... %d \n", argc > 1 ? definedPort : MYPORT);
        
        while(1) {  /* main accept() loop */
            sin_size = sizeof(struct sockaddr_in);
            if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
                                 &sin_size)) == -1) {
                perror("accept");
                continue;
            }
            printf("server: got connection from %s\n", \
                   inet_ntoa(their_addr.sin_addr));
            if (!fork()) { /* this is the child process */
                authenticateUser(new_fd);
            }
            close(new_fd);  /* parent doesn't need this */
            
            while(waitpid(-1,NULL,WNOHANG) > 0); /* clean up child processes */
        }
        
    }
}
