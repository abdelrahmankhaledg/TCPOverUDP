#include "readwrite.h"
#include "parser.h"
#include <chrono>
void * handleClient(void * thread_arg);

struct thread_data{
    int client_socket_fd;
    int * numOfConnectedClients;
    pthread_mutex_t * lock;
};