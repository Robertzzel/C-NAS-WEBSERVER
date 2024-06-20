#include "s_socket.h"
#include <pthread.h>

int PORT = 8001;
const char* HOST = "192.168.0.10";

void* myThreadFun(void *vargp)
{
    s_socket s;
    int err = socket_create(&s, IPv4, STREAM);
    if(err != 0){
        printf("server: cannot create socket");
        return NULL;
    }
    printf("server - Socket created\n");

    int port = PORT;
    err = socket_bind(&s, HOST, port);
    if(err != 0){
        printf("server: cannot bind socket");
        return NULL;
    }
    printf("server - Socket binded\n");

    err = socket_listen(&s, 1);
    if(err != 0){
        printf("server: cannot listen socket");
        return NULL;
    }
    printf("server - Socket listening\n");

    s_socket client;
    err = socket_accept(&s, &client);
    if(err != 0){
        printf("server: cannot accept socket");
        return NULL;
    }
    printf("server - Socket accepted\n");

    char buffer[1024] = "hello2";
    int buff_size = strlen(buffer);
    err = socket_write(&client, (uint8_t *) buffer, strlen(buffer), NULL);
    if(err != 0){
        printf("server: cannot write socket");
        return NULL;
    }

    int read;
    err = socket_read(&client, (uint8_t *) buffer, 1024, &read);
    if(err != 0){
        printf("server: cannot write socket");
        return NULL;
    }
    buffer[read] = '\0';
    printf("SERVER: MESSAGE: %s\n", buffer);
    return NULL;
}

int main() {
    pthread_t thread_id;
    printf("Before Thread\n");
    pthread_create(&thread_id, NULL, myThreadFun, NULL);
    printf("Thread created\n");

    sleep(1);
    s_socket s;
    int err = socket_create(&s, IPv4, STREAM);
    if(err != 0){
        printf("client: cannot create socket");
        return 1;
    }

    int port = PORT;
    err = socket_connect(&s, HOST, port);
    if(err != 0){
        printf("client: cannot connect socket\n");
        return 1;
    }

    char buffer[1024];
    int read;
    err = socket_read(&s, (uint8_t *) buffer, 1024, &read);
    if(err != 0){
        printf("client: cannot read socket");
        return 1;
    }
    buffer[read] = '\0';
    printf("client: MESSAGE: %s\n", buffer);

    memcpy(buffer, "hello_c", 8);
    int b_size = strlen(buffer);
    err = socket_write(&s, (uint8_t *) buffer, b_size, &read);
    if(err != 0){
        printf("client: cannot write socket");
        return 1;
    }

    pthread_join(thread_id, NULL);
}
