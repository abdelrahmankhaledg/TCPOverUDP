//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#include "ThreadFunction.h"

#define PORT 5050
#define NUM_THREADS 10
int main(int argc,char const *argv[]){
    int server_sock_fd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddrLen=sizeof(cliaddr);
    char buffer[1024]={0};
    pthread_t thread_id;
    //int numOfConnectedUsers=0;//TODO CHECK if this is desirable
    //pthread_mutex_t lock;
    //pthread_mutex_init(&lock,NULL);
    //Create the server socket

    cout<<"Creating Server Socket"<<endl;
    if((server_sock_fd=socket(AF_INET,SOCK_DGRAM,0))==0){
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }
    std::cout<<"Server Socket Has Been Created Successfully"<<endl;

    //Bind the socket to the local host ip address and 8080 port

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=INADDR_ANY;

    cout<<"Binding The Server Socket"<<endl;
    if((bind(server_sock_fd,(const struct sockaddr*)&servaddr,sizeof(servaddr)))<0){
        perror("Socket Binding Failed");
        exit(EXIT_FAILURE);
    }
    cout<<"Binding Server Socket is Complete"<<endl;


    while(1){

        int n;
        cout<<"Receiving the file name from the client"<<endl;
        n= recvfrom(server_sock_fd, (char *)buffer, sizeof (buffer), 0, (struct sockaddr *)&cliaddr,
                    (&cliaddrLen));
        buffer[n]='\0';
        //Create a thread to send/receive data with the client
        struct threadData * tdata=(struct threadData *) malloc(sizeof(struct threadData));
        tdata->server_sock_fd=server_sock_fd;
        tdata->cliaddr=cliaddr;
        strcpy(tdata->filePath,buffer);

        //pthread_mutex_lock(&lock);
        //  numOfConnectedUsers++;
        //tdata->numOfConnectedClients=&numOfConnectedUsers;
        //tdata->lock=&lock;
        //pthread_mutex_unlock(&lock);
        pthread_create(&thread_id,NULL,sendFile,(void *)tdata);
        pthread_detach(thread_id);
    }
    //pthread_mutex_destroy(&lock);
    return 0;
}
