#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "handleclient.h"

#define PORT 5050
#define NUM_THREADS 10
int main(int argc,char const *argv[]){
    int server_fd,new_socket;
    struct sockaddr_in address;
    int addrlen=sizeof(address);
    char buffer[1024]={0};
    pthread_t thread_id;
    int numOfConnectedUsers=0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock,NULL);
    //Create the server socket

    cout<<"Creating Server Socket"<<endl;
    if((server_fd=socket(AF_INET,SOCK_STREAM,0))==0){
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }
    std::cout<<"Server Socket Has Been Created Successfully"<<endl;

    //Bind the socket to the local host ip address and 8080 port

    address.sin_family=AF_INET;
    address.sin_port=htons(PORT);
    address.sin_addr.s_addr=INADDR_ANY;
    cout<<"Binding The Server Socket"<<endl;
    if((bind(server_fd,(struct sockaddr*)&address,addrlen))<0){
        perror("Socket Binding Failed");
        exit(EXIT_FAILURE);
    }
    cout<<"Binding Server Socket is Complete"<<endl;
    cout<<"Started Listening"<<endl;
        if(listen(server_fd,NUM_THREADS)<0){
            perror("Socket Listening Failed");
            exit(EXIT_FAILURE);
        }

    while(1){
        
        cout<<"Waiting to Accept The Client"<<endl;
        if((new_socket=accept(server_fd,(struct sockaddr*)&address,(socklen_t *)&addrlen))<=0){
            perror("AcceptError");
        }
        //Create a thread to send/receive data with the accepted socket
        struct thread_data * tdata=(struct thread_data *)malloc(sizeof(struct thread_data));
        tdata->client_socket_fd=new_socket;

        pthread_mutex_lock(&lock);
            numOfConnectedUsers++;
            tdata->numOfConnectedClients=&numOfConnectedUsers;
            tdata->lock=&lock;
        pthread_mutex_unlock(&lock);
        pthread_create(&thread_id,NULL,handleClient,(void *)tdata);
        pthread_detach(thread_id);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}