#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define PORT 12345
#define RANDOM_NUMBER_SEED 42
#define MAXDATASIZE 100

char Grid[10][10];
#define MAXSIZE 180

int queues[MAXSIZE];

int front = -1;
int rear = -1;
int size = -1;

int isempty()
{
    return size<0;
}

int isfull()
{
    return size == MAXSIZE;
}

void enqueue(int value)
{
    if(size<MAXSIZE)
    {
        if(size<0)
        {
            queues[0] = value;
            front = rear = 0;
            size = 1;
        }
        else if(rear == MAXSIZE-1)
        {
            queues[0] = value;
            rear = 0;
            size++;
        }
        else
        {
            queues[rear+1] = value;
            rear++;
            size++;
        }
    }
    else
    {
        printf("Queue is full\n");
    }
}

void dequeue()
{
    if(size<0)
    {
        printf("Queue is empty\n");
    }
    else
    {
        size--;
        front++;
    }
}

int Front()
{
    return queues[front];
}
struct Tile
{
    int adjacent_mines;
    bool revealed;
    bool isMine;
};
struct GameState
{
    struct Tile tiles[10][10];
} s;
void find_adjacent(int x,int y)
{
    int dy[]= {1,-1,0,0,1,-1,1,-1};
    int dx[]= {0,0,1,-1,1,-1,-1,1};
    int c=0;
    for(int i=0; i<8; i++)
    {
        if(x+dx[i]>=1 && x+dx[i]<=9 && y+dy[i]>=1 && y+dy[i]<=9)
        {
            if(s.tiles[x+dx[i]][y+dy[i]].isMine==true)
                c++;
        }
    }
    if(c>0)
    {
        Grid[x][y]=c+'0';
        s.tiles[x][y].revealed=true;
        return;
    }
    else
    {
        Grid[x][y]='0';
        s.tiles[x][y].revealed=true;
        for(int i=0; i<8; i++)
        {
            if(x+dx[i]>=1 && x+dx[i]<=9 && y+dy[i]>=1 && y+dy[i]<=9)
            {
                if(s.tiles[x+dx[i]][y+dy[i]].isMine==false && s.tiles[x+dx[i]][y+dy[i]].revealed==false)
                {
                    find_adjacent(x+dx[i],y+dy[i]);
                }
            }
        }
    }
    
}
void expand(int x,int y)
{
    int dy[]= {1,-1,0,0,1,-1,1,-1};
    int dx[]= {0,0,1,-1,1,-1,-1,1};
    enqueue(x);
    enqueue(y);
    while(!isempty())
    {
        int a=Front();
        dequeue();
        int b=Front();
        dequeue();
        int c=0;
        for(int i=0; i<8; i++)
        {
            
            if(a+dx[i]>=1 && a+dx[i]<=9 && b+dy[i]>=1 && b+dy[i]<=9)
            {
                if(s.tiles[a+dx[i]][b+dy[i]].isMine==true)
                {
                    c++;
                }
            }
        }
        Grid[a][b]=c+'0';
        if(c==0)
        {
            Grid[a][b]='0';
            for(int i=0; i<8; i++)
            {
                
                if(a+dx[i]>=1 && a+dx[i]<=9 && b+dy[i]>=1 && b+dy[i]<=9)
                {
                    enqueue(a+dx[i]);
                    enqueue(b+dy[i]);
                }
            }
        }
        else
        {
            return;
        }
    }
}
void place_mine()
{
    for(int i=0; i<10; i++)
    {
        int x,y;
        do
        {
            x=1+rand()%9;
            y=1+rand()%9;
        }
        while(s.tiles[x][y].isMine==true);
        s.tiles[x][y].isMine=true;
    }
}
void printBoard()
{
    for(int i=0; i<=9; i++)
    {
        if(i>0)
        {
            char x='A'+i-1;
            printf("%c", x);
        }
        for(int j=0; j<=9; j++)
        {
            if(i==0)
            {
                if(j==0)
                    printf("  ");
                else
                {
                    printf("%d", j);
                }
            }
            else
            {
                printf("%c", Grid[i][j]);
            }
        }
        printf("\n");
    }
}
bool complete()
{
    for(int i=1; i<=9; i++)
    {
        for(int j=1; j<=9; j++)
        {
            if(s.tiles[i][j].revealed==false && s.tiles[i][j].isMine==false)
                return false;
        }
    }
    return true;
    
}
bool winner()
{
    for(int i=1; i<=9; i++)
    {
        for(int j=1; j<=9; j++)
        {
            if(Grid[i][j]!='+' && s.tiles[i][j].isMine)
                return false;
        }
    }
    return true;
}
void startGame(){
    place_mine();
    clock_t start=clock();
    while(!complete() && !winner())
    {
        printBoard();
        printf("\n");
        printf("Choose an Option:\n");
        printf("<R> Reveal Tile\n");
        printf("<P> Put Flag\n");
        printf("<Q> Quit\n");
        printf("Option(R,P,Q):");
        char o;
        scanf("%c", &o);
        if(o=='R')
        {
            char a;
            int x,y;
            printf("Enter Tile Cordinate:");
            char st[2];
            scanf("%s",st);
            getchar();
            x=st[0]-64;
            y=st[1]-'0';
            if(s.tiles[x][y].revealed==true)
            {
                printf("Error: Already Revealed\n");
            }
            else if(s.tiles[x][y].isMine==true)
            {
                printf("Game Over");
                break;
            }
            expand(x,y);
            front=-1;
            rear=-1;
            size=-1;
            s.tiles[x][y].revealed=true;
        }
        else if(o=='P')
        {
            char a;
            int x,y;
            printf("Input:");
            char st[2];
            scanf("%s",st);
            getchar();
            x=st[0]-64;
            y=st[1]-'0';
            Grid[x][y]='+';
        }
        else if(o=='Q')
        {
            break;
        }
    }
    double duration=clock()-start;
    if(winner())
    {
        printf("Congratulations! You have Located all mines.");
        printf("You won in %f seconds!\n", duration/1000);
    }
}

void introGame(){
    puts("=====================================================");
    puts("Welcome to the online Minesweeper gaming system");
    puts("=====================================================\n");
    puts("You are required to log on with your registered username and password.\n");
}
int authenticateUser(int sockfd){
    char username[100], password[100];
    char reply[50];
    memset(username, 0 , 100);
    memset(password, 0 , 100);
    char failure[100] = "You entered an incorrect username or password. Disconnecting.";
    printf("Username : ");
    scanf("%s" , username);
    getchar();
    printf("Password : ");
    scanf("%s" , password);
    getchar();
    send(sockfd , username , strlen(username) , 0);
    send(sockfd , password , strlen(password) , 0);
    recv(sockfd , reply , 1 , 0);
    if(strcmp(reply, "1") == 0){
        return 1;
    }
    else{
        printf("%s\n", failure);
        return 0;
    }
}
void showLeaderboard(int sockfd){
    char getBoard[30];
    char result[50];
    strcpy(getBoard, "showLeaderboard");
    send(sockfd , getBoard , strlen(getBoard) , 0);
    recv(sockfd , result , 2000 , 0);
    puts("\n==============================================================================");
    if(strcmp(result, "1") == 0)
        puts("Results recieved from server!");
    else{
        puts("There is no information currently stored in the leaderboard. Try again later.");
    }
    puts("==============================================================================\n");
}
void gameEntry(int sockfd){
    int input;
    printf("\nWelcome to the Minesweeper gaming system.\n");
    while (1) {
    printf("\nPlease enter a selection:\n");
    printf("<1> Play Minesweeper\n");
    printf("<2> Show Leaderboard\n");
    printf("<3> Quit\n");
    printf("Selection option(1-3):");
    scanf("%d", &input);
    switch (input) {
        case 1:
            startGame();
            break;
        case 2:
            showLeaderboard(sockfd);
            break;
        case 3:
            exit(1);
            break;
        default:
            break;
    }
    }
    
}

int main(int argc, char *argv[])
{
    srand(RANDOM_NUMBER_SEED);
    int sockfd;
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */
    
    if (argc != 3) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    
    if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
        herror("gethostbyname");
        exit(1);
    }
    int definedPort = 0;
    if(argc > 2){
        definedPort = atoi(argv[2]);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    
    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(definedPort);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */
    
    if (connect(sockfd, (struct sockaddr *)&their_addr, \
                sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }
    introGame();
    while(1)
    {
        int authenticated;
        authenticated = authenticateUser(sockfd);
        if(!authenticated){
            exit(1);
        }
        gameEntry(sockfd);
        break;
    }
    
    close(sockfd);
    
    return 0;
}

