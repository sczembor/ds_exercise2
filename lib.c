#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "lib.h"
#include <stdlib.h>

#define MAX_LINE     256

struct msgs{
    int type;
    char* key;
    char* val1;
    int val2;
    float val3;
    struct msgs* pNext;
};

int sendMessage(int socket, char * buffer, int len)
{
    int r;
    int l = len;
    
    do {
        r = write(socket, buffer, l); //Writes into the socket!!!!!
        //printf("written into the socket\n");
        l = l -r;
        buffer = buffer + r;
        //printf("WTF\n");
    } while ((l>0) && (r>=0));
    
    if (r < 0)
        return (-1);   /* fail */
    else
        return(0);    /* full length has been sent */
}

int recvMessage(int socket, char *buffer, int len)
{
    int r;
    int l = len;
    
    
    do {
        r = read(socket, buffer, l); //Reads from the socket!!!!
        l = l -r ;
        buffer = buffer + r;
    } while ((l>0) && (r>=0));
    
    if (r < 0)
        return (-1);   /* fallo */
    else
        return(0);    /* full length has been receive */
}



ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;  /* num of bytes fetched by last read() */
    size_t totRead;      /* total bytes read so far */
    char *buf;
    char ch;
    
    
    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    buf = buffer;
    totRead = 0;
    
    for (;;) {
        numRead = read(fd, &ch, 1);    /* read a byte */
        
        if (numRead == -1) {
            if (errno == EINTR)    /* interrupted -> restart read() */
                continue;
            else
                return -1;        /* some other error */
        } else if (numRead == 0) {    /* EOF */
            if (totRead == 0)    /* no byres read; return 0 */
                return 0;
            else
                break;
        } else {            /* numRead must be 1 if we get here*/
            if (ch == '\n')
                break;
            if (ch == '\0')
                break;
            if (totRead < n - 1) {        /* discard > (n-1) bytes */
                totRead++;
                *buf++ = ch;
            }
        }
    }
    
    *buf = '\0';
    return totRead;
}

void input_send(int socket){
    int n;
    char buffer[MAX_LINE];
    n = readLine(0, buffer, MAX_LINE);
    if (n!=-1){
        int msg;
        msg=sendMessage(socket, buffer, n+1);
        //printf("Message sent: %s\n",buffer);
        if (msg < 0) {
            perror("Error in sending msg");
            exit(1);
        }
    }
}
void send_type(int socket,int type){
        int msg;
        char buffer[MAX_LINE];
        sprintf(&buffer,"%d",type);
        msg=sendMessage(socket, buffer, 2);
        //printf("Message sent: %s\n",buffer);
        if (msg < 0) {
            perror("Error in sending msg");
            exit(1);
    }
}
void recieve_values(int socket){
    char buffer[MAX_LINE];
    int msg = readLine(socket, buffer, MAX_LINE);
    printf("Value 1: %s\n",buffer);
    if (msg < 0) {
        perror("Error in recieving msg");
        exit(1);
    }
    msg = readLine(socket, buffer, MAX_LINE);
    printf("Value 2: %s\n",buffer);
    if (msg < 0) {
        perror("Error in recieving msg");
        exit(1);
    }
    msg = readLine(socket, buffer, MAX_LINE);
    printf("Value  3: %s\n",buffer);
    if (msg < 0) {
        perror("Error in recieving msg");
        exit(1);
    }
}

int recieve_result(int socket){
    char buffer[MAX_LINE];
    int msg = readLine(socket, buffer, MAX_LINE);
    printf("Function that you selected returned: %s\n",buffer);
    if (msg < 0) {
        perror("Error in recieving msg");
        exit(1);
    }
    int res = atoi(buffer);
    return  res;
}

int init(int socket){
    send_type(socket,1);
    return recieve_result(socket);
}
int set_value(int socket){
    send_type(socket,2);
    printf("key:\n");
    input_send(socket);
    printf("val1:\n");
    input_send(socket);
    printf("val2:\n");
    input_send(socket);
    printf("val3:\n");
    input_send(socket);
    return recieve_result(socket);
}
int get_value(int socket){
    send_type(socket,3);
    printf("key:\n");
    input_send(socket);
    int res = recieve_result(socket);
    if (res==0){
       recieve_values(socket);
       return res;
    }
    else
    {
        return -1;
    }
}
int modify_value(int socket){
    send_type(socket,4);
    printf("key:\n");
    input_send(socket);
    printf("val1:\n");
    input_send(socket);
    printf("val2:\n");
    input_send(socket);
    printf("val3:\n");
    input_send(socket);
    return recieve_result(socket);
}
int delete_key(int socket){
    send_type(socket,5);
    printf("key:\n");
    input_send(socket);
    return recieve_result(socket);
}
int exist(int socket){
    send_type(socket,6);
    printf("key:\n");
    input_send(socket);
    return recieve_result(socket);
}
int num_items(int socket){
    send_type(socket,7);
    return recieve_result(socket);
}
int terminate(int socket){
    send_type(socket,8);
    close(socket);
    return(0);
}
