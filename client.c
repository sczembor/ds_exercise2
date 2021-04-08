#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "lib.h"

#define MAX_LINE     256
/*
struct msgs{
    int type;
    char* key;
    char* val1;
    int val2;
    float val3;
    struct msgs* pNext;
};
 */
void input_send(int socket);

int main(int argc, char *argv[])
{
    //struct msgs msg;
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    
    if (argc != 3) {
        printf("Usage: client <serverAddress> <serverPort>\n");
        exit(0);
    }
    
    // the socket is created
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0){
        perror("Error in socket");
        exit(1);
    }
    
    // Obtain Server address
    bzero((char *)&server_addr, sizeof(server_addr));
    hp = gethostbyname (argv[1]);
    if (hp == NULL) {
        perror("Error en gethostbyname");
        exit(1);
    }
    
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    
    // Complete.....
    // First: establish the connection
    int connection = connect (sd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if (connection==0){
        printf("Connection established congrats!\n");
    }
    else{
        printf("NOT CONNECTED\n");
    }
    
    int err = 0;
    int msgtype;
    while (err != -1) {
        scanf("%i", &msgtype);
        switch (msgtype) {
            case 1:
                input_send(sd);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
        }
    }
    
    
    return (sd);
    
    return(0);
}

void input_send(int socket){
    int n;
    char buffer[MAX_LINE];
    n = readLine(0, buffer, MAX_LINE);
    if (n!=-1){
        int msg;
        msg=sendMessage(socket, buffer, n+1);
        printf("Message sent: %s\n",buffer);
        if (msg < 0) {
            perror("Error in sending msg");
            exit(1);
        }
    }
}

