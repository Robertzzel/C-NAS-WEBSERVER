#include "m_socket.h"

int main() {
    int err;
    m_socket s;
    err = socket_create(&s, IPv4, STREAM);
    if(err != 0){
        return 1;
    }

    err = socket_connect(&s, "127.0.0.1", 8000);
    if(err != 0){
        return 1;
    }

    char buffer[1024] = "hello";
    uint64_t read;
    socket_read(&s, (uint8_t*)buffer, 1024, &read);
    buffer[read] = '\0';
    socket_write(&s, (uint8_t*)buffer, strlen(buffer), NULL);

    printf("Message: %s", buffer);

    err = socket_close(&s);
    if(err != 0){
        return 1;
    }

    return 0;
}
