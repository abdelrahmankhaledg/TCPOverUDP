//
// Created by abdelrahmankhaledg on ٥‏/١‏/٢٠٢٢.
//
#include "ThreadFunction.h"
#define TIMEOUT 1

void* sendFile(void * thread_arg){
    struct threadData* data=(struct threadData * )thread_arg;
    int server_sock_fd;
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

    FILE *log;
    log = fopen("/home/abdelrahmankhaledg/CLionProjects/untitled/log", "a");
    if((server_sock_fd=socket(AF_INET,SOCK_DGRAM,0))==0){
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }





    int sendRound=1;
    while (fileSize>0) {
        int readDataSize = min(SIZE, fileSize);
        fileSize -= readDataSize;
        char readData[readDataSize];
       // cout<<fileSize<<endl;
        fread(readData, readDataSize, 1, fp);


        Packet *packet = new Packet;
        memset(packet->data,0,500);
        memcpy(packet->data, readData,readDataSize);

        packet->len = readDataSize + 6;//TODO Check
        packetsQueue.push(packet);
        int in=0;
        if (packetsQueue.size() == data->cwnd->currentAvailableCwndSize || fileSize==0) {

            if(in==0){
                if(data->cwnd->state==1){
                  //  cout<<data->cwnd->currentAvailableCwndSize<<endl;
                    data->cwnd->currentAvailableCwndSize=(data->cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(data->cwnd->currentAvailableCwndSize+1);
                }
                fprintf(log,"Transmission Round %d has cwnd size of %d\n",sendRound,data->cwnd->currentAvailableCwndSize);
                sendRound++;
            }
            in=1;
            timeval timeout;
            setTimeOut(&timeout,TIMEOUT);
            int ackResult=-1;
            do{

                FD_ZERO(&readfds);
                FD_SET(server_sock_fd, &readfds);
                addPacketsToCwnd(&packetsQueue, data->cwnd);
                if(ackResult!=0)
                sendPackets(data->cwnd, server_sock_fd, data->cliaddr);
                //writePackets(data->cwnd, data->server_sock_fd, data->cliaddr);

                int sret = select(server_sock_fd + 1, &readfds, 0, 0, &timeout);
                if (sret == -1) {
                    perror("Error happened in select");
                    break;
                } else if (sret == 0) {//Timeout occured
                   // cout<<"Timeout Occurred On Packet of Seqno "<<data->cwnd->packetsWindow[0]->seqno<<endl;
                    //resend all the packets again
                    changeStateOnTimeout(data->cwnd);
                    setTimeOut(&timeout,TIMEOUT);
                    ackResult=-1;
                } else {//There is an acknowledge packet that can be received from the client
                    //receive the packet
                    char buf[ackPacketSize];
                    socklen_t cliaddLen = sizeof(data->cliaddr);
                    recvfrom(server_sock_fd, (char *) buf, ackPacketSize, 0, (struct sockaddr *) &(data->cliaddr), (&(cliaddLen)));
                    Ack_packet *ackPacket = new Ack_packet;
                    deserializeAckPacket(buf, ackPacket);
                   // cout<<ackPacket->ackno<<endl;
                     ackResult = controlFlow(ackPacket, data->cwnd);
                    if (ackResult == -1 && data->cwnd->duplicateAcknowledgment != 3) {//wait on timeout again
                       // cout<<"A Duplicate Acknowledgement Has Been Received"<<endl;
                        ackResult=0;
                    } else if (ackResult == -1 && data->cwnd->duplicateAcknowledgment == 3) {//fast recovery retransmit all packets
                        setTimeOut(&timeout,TIMEOUT);
                       // cout<<"Packets Will Be Retransmitted due to 3 Duplicate Acknowledgements"<<endl;
                    }

                    else if (ackResult == 0) {//ack packet is not a duplicate

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
    fclose(log);
    //send packet to client so that it closes
    Packet* closePacket =new Packet;
    //strcpy(closePacket->data,"CLS");
    closePacket->seqno=65;
    char serializedPacket[sizeof(Packet)];
    serializePacket(closePacket,serializedPacket);
   // cout<<"Sending the close packet"<<endl;
    sendto(server_sock_fd,serializedPacket, sizeof(serializedPacket),0,(struct sockaddr* )&data->cliaddr,sizeof(data->cliaddr));
}

void setTimeOut(timeval* timeout,int timeoutVal){
    timeout->tv_usec=0;
    timeout->tv_sec=timeoutVal;
}