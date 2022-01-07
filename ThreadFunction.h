//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_THREADFUNCTION_H
#define UNTITLED_THREADFUNCTION_H



#include <queue>
#include "Packet.h"
#include "cwnd.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
using namespace std;
#define SIZE 500
#define ackPacketSize sizeof(Ack_packet)

struct threadData{
    char filePath[100];// TODO Check
    sockaddr_in cliaddr;
    struct cwnd* cwnd;
};


void* sendFile(void * thread_arg);
void setTimeOut(timeval* timeout,int timeoutVal);


#endif //UNTITLED_THREADFUNCTION_H
