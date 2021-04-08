
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lib.h"

#define MAX_LINE    256
#define TRUE 1
#define FALSE 0

struct msgs{
    int type;
    char key[254];
    char val1[254];
    int val2;
    float val3;
    struct msgs* pNext;
};

int busy;
int kill;
pthread_t thread;
pthread_attr_t attr;
pthread_mutex_t mutex1;
pthread_cond_t signal1;

//FUNCTIONS
void manage_request (int *s) {
    struct msgs msg;
    int sc;
    kill=FALSE;
    pthread_mutex_lock(&mutex1);
    char buffer[MAX_LINE];
    int n;
    
    printf("thread connected as well GJ\n");
    sc=*s;
    busy=FALSE;
    pthread_cond_signal(&signal1);
    pthread_mutex_unlock(&mutex1);
    
    while (1){
        
        int msg = readLine(sc, buffer, MAX_LINE);
        printf("Message recieved: %s\n",buffer);
        if (msg < 0) {
            perror("Error in recieving msg");
            exit(1);
        }
        /*
        int mes = recvMessage(sc, msg.type, MAX_LINE);
        printf("Message recieved: %s\n",msg.type);
        if (mes < 0) {
            perror("Error in recieving msg");
            exit(1);
        }
        
        mes = recvMessage(sc, msg.key, MAX_LINE);
        printf("Message recieved: %s\n",msg.key);
        if (mes < 0) {
            perror("Error in recieving msg");
            exit(1);
        }
        
        mes = recvMessage(sc, msg.type, MAX_LINE);
        printf("Message recieved: %s\n",msg.val1);
        if (mes < 0) {
            perror("Error in recieving msg");
            exit(1);
        }
              
        mes = recvMessage(sc, msg.val2, MAX_LINE);
        printf("Message recieved: %s\n",msg.val2);
        if (mes < 0) {
            perror("Error in recieving msg");
            exit(1);
        }
        mes = recvMessage(sc, (char) msg.val3, MAX_LINE);
        printf("Message recieved: %s\n",msg.val3);
               if (mes < 0) {
                   perror("Error in recieving msg");
                   exit(1);
               }
         */
        
        /*
        if(strncmp(buffer,"EXIT",4)==0){
            break;
        }
        else if(strncmp(buffer,"KILL",4)==0){
            kill = TRUE;
            printf("Server shutting down\n");
            exit(1);
        }
        else{
            msg=sendMessage(sc, buffer, msg+1);
            //printf("Message sending in progress\n");
            if (msg < 0) {
                perror("Error in sending msg\n");
                exit(1);
            }
            printf("Message sent: %s\n",buffer);
        }
        */
    }
    printf("WARNING: EXIT command recieved, THREAD DISCONNECTING\n");
    close(sc);
    pthread_exit(NULL);
}

//MAIN
int main(int argc, char *argv[])
{
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&mutex1,NULL);
    pthread_cond_init(&signal1,NULL);
    
    int sd;
    int val;
    socklen_t size;
    struct sockaddr_in server_addr,client_addr;
    int err;
    
    sd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("Error in socket");
        exit(1);
    }
    
    val = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val,
                     sizeof(int));
    if (err < 0) {
        perror("Error in opction");
        exit(1);
    }
    
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4200);
    
    // Complete .....
    int connection;
    connection = bind(sd,&server_addr,sizeof(server_addr));
    if (connection < 0){
        perror("Error in connecting");
        exit(1);
    }
    
    connection = listen(sd,5);
    if (connection < 0) {
        perror("Error in connecting");
        exit(1);
    }
    
    while(1){
        printf("waiting  for connection\n");
        int sc = accept(sd,(struct sockaddr *)&client_addr,&size);
        pthread_create(&thread,&attr,manage_request,&sc); //HERE!!!!!
        pthread_mutex_lock(&mutex1);
        while(busy==TRUE){
            pthread_cond_wait(&mutex1,&signal1);
        }
        pthread_mutex_unlock(&mutex1);
        busy=TRUE;
        if (sc < 0){
            perror("NOT CONNECTED");
            exit(1);
        }
        else{
            printf("connected\n");
        }
        if (kill==TRUE){
            exit(1);
        }
    }
    close (sd);
    return(0);
}

