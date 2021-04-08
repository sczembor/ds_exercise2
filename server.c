
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

struct Element{
    int type;
    char key[254];
    char value1[254];
    int value2;
    float value3;
    struct Element* pNext;
};
//GLOBALS ----------
int busy;
int kill;
pthread_t thread;
pthread_attr_t attr;
pthread_mutex_t mutex1;
pthread_cond_t signal1;
struct Element* pHead = NULL;

//FUNCTIONS  DECLARATIONS  -------------
int addNode(char* key, char* value1, int* value2, float* value3);
int modifyNode(char* key, char* value1, int* value2, float* value3);
int deleteList(void);
int searchList(char* _key);
int deleteElement(char* key);
struct Element* getValue(char* key);
int numElements(void);

//FUNCTIONS
void manage_request (int *s) {
    struct Element tmp;
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
        int type = atoi(buffer);
        int toReturn;
        switch (type) {
            case 1://init
                toReturn = deleteList();
                sprintf(&buffer, "%d", toReturn);
                msg = sendMessage(sc, buffer, strlen(buffer)+1);
                if (msg < 0) {
                    perror("Error in sending msg");
                    exit(1);
                }
                printf("function returned:%i\n",toReturn);
                break;
            case 2://set_value
                msg = readLine(sc, buffer, MAX_LINE);//key
                printf("Message recieved: %s\n",buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                strcpy(tmp.key, buffer);
                msg = readLine(sc, buffer, MAX_LINE);//value1
                printf("Message recieved: %s\n",buffer);
                strcpy(tmp.value1, buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                msg = readLine(sc, buffer, MAX_LINE);//value2
                printf("Message recieved: %s\n",buffer);
                tmp.value2 = atoi(buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                msg = readLine(sc, buffer, MAX_LINE);//value3
                printf("Message recieved: %s\n",buffer);
                tmp.value3 = atof(buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                printf("key:%s\n", tmp.key);
                printf("value1:%s\n", tmp.value1);
                printf("value2:%i\n", tmp.value2);
                printf("value3:%f\n", tmp.value3);
                if(searchList(&tmp.key)==0)
                {
                    toReturn = addNode(&tmp.key,&tmp.value1,&tmp.value2,&tmp.value3);
                }
                else
                {
                    toReturn = -1;
                }
                sprintf(&buffer, "%d", toReturn);
                printf("What i send back from set_value%s\n",buffer);
                msg = sendMessage(sc, buffer, strlen(buffer)+1);
                if (msg < 0) {
                    perror("Error in sending msg");
                    exit(1);
                }
                break;
            case 3://get_value
                msg = readLine(sc, buffer, MAX_LINE);//key
                printf("Message recieved: %s\n",buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                strcpy(tmp.key, buffer);
                if(searchList(&tmp.key)==1)
                {
                    printf("im in the list yay\n");
                    toReturn = 0;
                    sprintf(&buffer, "%d", toReturn);
                    printf("toReturn:%s\n",buffer);
                    msg = sendMessage(sc, buffer, strlen(buffer)+1);
                    if (msg < 0) {
                        perror("Error in sending msg");
                        exit(1);
                    }
                    tmp = *getValue(tmp.key);
                    strcpy(buffer, tmp.value1);
                    printf("value1:%s\n",buffer);
                    msg = sendMessage(sc, buffer, strlen(buffer)+1);//value1
                    if (msg < 0) {
                        perror("Error in sending msg");
                        exit(1);
                    }
                    sprintf(&buffer, "%d", tmp.value2);
                    printf("value2:%s\n",buffer);
                    msg = sendMessage(sc, buffer, strlen(buffer)+1);//value2
                    if (msg < 0) {
                        perror("Error in sending msg");
                        exit(1);
                    }
                    sprintf(&buffer, "%f", tmp.value3);
                    printf("value3:%s\n",buffer);
                    msg = sendMessage(sc, buffer, strlen(buffer)+1);//value3
                    if (msg < 0) {
                        perror("Error in sending msg");
                        exit(1);
                    }
                }
                else
                {
                    printf("im not in the list boo\n");
                    toReturn = -1;
                    sprintf(&buffer, "%d", toReturn);
                    msg = sendMessage(sc, buffer, strlen(buffer)+1);
                    if (msg < 0) {
                        perror("Error in sending msg");
                        exit(1);
                    }
                }
                break;
            case 4://modify value
                msg = readLine(sc, buffer, MAX_LINE);//key
                printf("Message recieved: %s\n",buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                strcpy(tmp.key, buffer);
                msg = readLine(sc, buffer, MAX_LINE);//value1
                printf("Message recieved: %s\n",buffer);
                strcpy(tmp.value1, buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                msg = readLine(sc, buffer, MAX_LINE);//value2
                printf("Message recieved: %s\n",buffer);
                tmp.value2 = atoi(buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                msg = readLine(sc, buffer, MAX_LINE);//value3
                printf("Message recieved: %s\n",buffer);
                tmp.value3 = atof(buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                printf("key:%s\n", tmp.key);
                printf("value1:%s\n", tmp.value1);
                printf("value2:%i\n", tmp.value2);
                printf("value3:%f\n", tmp.value3);

                toReturn = modifyNode(&tmp.key, &tmp.value1,&tmp.value2,&tmp.value3);

                sprintf(&buffer, "%d", toReturn);
                msg = sendMessage(sc, buffer, 2);
                if (msg < 0)
                {
                    perror("Error in sending msg");
                    exit(1);
                }

                break;
            case 5://delete_key
                printf("function returned:%i\n",type);
                msg = readLine(sc, buffer, MAX_LINE);
                printf("Message recieved: %s\n",buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                strcpy(tmp.key, buffer);
                toReturn = deleteElement(&tmp.key);

                sprintf(&buffer, "%d", toReturn);
                msg = sendMessage(sc, buffer, strlen(buffer)+1);
                if (msg < 0)
                {
                    perror("Error in sending msg");
                    exit(1);
                }

                break;
            case 6://exsist
                printf("function returned:%i\n",type);
                msg = readLine(sc, buffer, MAX_LINE);
                printf("Message recieved: %s\n",buffer);
                if (msg < 0) {
                    perror("Error in recieving msg");
                    exit(1);
                }
                strcpy(tmp.key, buffer);
                toReturn = searchList(&tmp.key);

                sprintf(&buffer, "%d", toReturn);
                msg = sendMessage(sc, buffer, strlen(buffer)+1);
                if (msg < 0)
                {
                    perror("Error in sending msg");
                    exit(1);
                }
                break;
            case 7://num_items

                toReturn = numElements();

                sprintf(&buffer, "%d", toReturn);
                msg = sendMessage(sc, buffer, strlen(buffer)+1);
                if (msg < 0)
                {
                    perror("Error in sending msg");
                    exit(1);
                }
                break;
            default:
                perror ("Client: Invalid Arguemnt(function)");
                exit (1);
                break;
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
int addNode(char* key, char* value1, int* value2, float* value3)
{
    struct Element* new = (struct Element*)malloc(sizeof(struct Element));
    strcpy(new->key,key);
    strcpy(new->value1,value1);
    new->value2 = *value2;
    new->value3 = *value3;
    new->pNext = pHead;
    pHead = new;
    return 0;
}
int deleteList()
{
    struct Element* tmp = NULL;
    while(pHead != NULL){
        tmp = pHead->pNext;
        free(pHead);
        pHead = tmp;
    }
    return 0;
}
int searchList(char* _key)
{
    struct Element* tmp = pHead;
    while(tmp != NULL)
    {
        if(strcmp(_key, tmp->key) == 0)
            return 1;
        tmp = tmp->pNext;
    }
    return 0;//element does not exsist
}
struct Element* getValue(char* key)
{
    struct Element* tmp = pHead;
    while(tmp != NULL)
    {
        if(!strcmp(key, tmp->key))
            return tmp;
        tmp = tmp->pNext;
    }
    return NULL;//element does not exsist
}
int modifyNode(char* key, char* value1, int* value2, float* value3)
{
    struct Element* tmp = pHead;
    while(tmp != NULL)
    {
        if(!strcmp(key, tmp->key))
        {
            strcpy(tmp->value1, value1);
            tmp->value2 = *value2;
            tmp->value3 = *value3;
            return 0;
        }
        tmp = tmp->pNext;
    }
    return -1;//element does not exsist
}
int deleteElement(char* key)
{
    struct Element* prev = NULL;
    struct Element* tmp = pHead;
    while(tmp)
    {
        if(!strcmp(key, tmp->key))
        {
            if(prev!=NULL)
                prev->pNext = tmp->pNext;
            else
                pHead = tmp->pNext;
            free(tmp);
            return 0;
        }
        prev = tmp;
        tmp = tmp->pNext;
    }
    return -1;//element does not exsist
}
int numElements()
{
    int num = 0;
    struct Element* tmp = pHead;
    while(tmp)
    {
        num = num + 1;
        tmp = tmp->pNext;
    }
    return num;
}

