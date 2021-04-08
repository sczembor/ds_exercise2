#include <unistd.h>


int recvMessage(int socket, char *buffer, int len);
int sendMessage(int socket, char * buffer, int len);
ssize_t readLine(int fd, void *buffer, size_t n);

void input_send(int socket);
void send_type(int socket,int type);

int init(int socket);
int set_value(int socket);
int get_value(int socket);
int modify_value(int socket);
int delete_key(int socket);
int exist(int socket);
int num_items(int socket);
