//
// Created by abdelrahmankhaledg on ٥‏/١‏/٢٠٢٢.
//
#include "ThreadFunction.h"

void* sendFile(void * thread_arg){
    struct threadData* data=(struct threadData * )thread_arg;
    fd_set readfds;
    FILE * fp;
    queue<Packet *> packetsQueue;
    fp=fopen(data->filePath,"r");
    int fileSize;
    fseek(fp,0,SEEK_END);
    fileSize=ftell(fp);
    fseek(fp,0,SEEK_SET);
    data->cwnd=new struct cwnd;
    initializeCwnd(data->cwnd);

    //TODO handle 500 bytes of packet aren't all full with data
    while (fileSize>0) {
        int readDataSize = min(SIZE, fileSize);
        fileSize -= readDataSize;
        char readData[readDataSize];

        fread(readData, readDataSize, 1, fp);

        Packet *packet = new Packet;
        memset(packet->data,0,500);
        strncpy(packet->data, readData,readDataSize);
        packet->len = readDataSize + 6;//TODO Check
        packetsQueue.push(packet);
        if (packetsQueue.size() == data->cwnd->currentAvailableCwndSize || fileSize==0) {
            timeval timeout;
            setTimeOut(&timeout,1);
            int ackResult=-1;
            do {
                FD_ZERO(&readfds);
                FD_SET(data->server_sock_fd, &readfds);
                addPacketsToCwnd(&packetsQueue, data->cwnd);
                if(ackResult!=0)
                sendPackets(data->cwnd, data->server_sock_fd, data->cliaddr);

                int sret = select(data->server_sock_fd + 1, &readfds, NULL, NULL, &timeout);
                if (sret == -1) {
                    perror("Error happened in select");
                    break;
                } else if (sret == 0) {//Timeout occured
                    //resend all the packets again
                    changeStateOnTimeout(data->cwnd);
                    setTimeOut(&timeout,1);
                } else {//There is an acknowledge packet that can be received from the client
                    //receive the packet
                    char buf[ackPacketSize];
                    socklen_t cliaddLen = sizeof(data->cliaddr);
                    recvfrom(data->server_sock_fd, (char *) buf, ackPacketSize, 0, (struct sockaddr *) &(data->cliaddr), (&(cliaddLen)));
                    Ack_packet *ackPacket = new Ack_packet;
                    deserializeAckPacket(buf, ackPacket);
                     ackResult = controlFlow(ackPacket, data->cwnd);
                    if (ackResult == -1 && data->cwnd->duplicateAcknowledgment < 3) {//wait on timeout again

                    } else if (ackResult == -1 && data->cwnd->duplicateAcknowledgment == 3) {//fast recovery retransmit all packets
                        setTimeOut(&timeout,1);
                    } else if (ackResult == 0) {//ack packet is not a duplicate
                        if(data->cwnd->packetsWindow.size()==0){
                            break;
                        }//all packets have been acknowledged
                    }

                }
            } while(true);
        }
    }

    //Send the last part of the data and clear the buffer
    if(fp!=NULL){
        fclose(fp);
    }
    //send packet to client so that it closes
    Packet* closePacket =new Packet;
    strcpy(closePacket->data,"CLS");
    char serializedPacket[sizeof(Packet)];
    serializePacket(closePacket,serializedPacket);
    sendto(data->server_sock_fd,serializedPacket, sizeof(serializedPacket),0,(struct sockaddr* )&data->cliaddr,sizeof(data->cliaddr));
}

void setTimeOut(timeval* timeout,int timeoutVal){
    timeout->tv_usec=0;
    timeout->tv_sec=timeoutVal;
}