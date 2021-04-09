#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "lib.h"

int main(int argc, char *argv[])
{
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
        printf("1. init()\n2.set_value()\n3.get_value()\n4.modify_value()\n5.delete_key()\n6.exist()\n7.num_items()\n8.exit()\nChoose one option(ex. \"1\"):");
        scanf("%i", &msgtype);
        switch (msgtype) {
            case 1:
                init(sd);
                break;
            case 2:
                set_value(sd);
                break;
            case 3:
                get_value(sd);
                break;
            case 4:
                modify_value(sd);
                break;
            case 5:
                delete_key(sd);
                break;
            case 6:
                exist(sd);
                break;
            case 7:
                num_items(sd);
                break;
            case 8:
                terminate(sd);
                exit(1);
                break;
        }
    }
    
    
    return (sd);
    
    return(0);
}


