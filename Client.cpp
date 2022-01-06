//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <fstream>
#include "Ack_packet.h"
#include "Packet.h"
#include "serialize_deserialize.h"
#define PORT 5050
#define bufferSize sizeof(Packet)
using namespace std;


uint32_t ExpectedSequenceNumber = 0;
string IPAddressServer = "", PortNumber = "";
const char *FileName = "";

int main(int argc, char const *argv[]) {
    priority_queue<int> priorityQ; //priority queue to arrange the arriving packets

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[bufferSize] = {0};
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }


/*
    int counter = 1;
    std::ifstream file("/home/vortex-ubuntu/CLionProjects/client/client.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            // using printf() in all tests for consistency
            if (counter == 1) {
                IPAddressServer = line.c_str();
            } else if (counter == 2) {
                PortNumber = line.c_str();
            } else {
                FileName = line.c_str();
            }
            printf("%s\n", line.c_str());
            counter++;
        }
        file.close();
    }*/
    FileName="/home/abdelrahmankhaledg/CLionProjects/untitled/test2";
    FILE *pFileTXT;
    pFileTXT = fopen("/home/abdelrahmankhaledg/CLionProjects/untitled1/output", "w");
    sendto(sock, FileName, strlen(FileName), 0,(struct sockaddr * )&serv_addr,sizeof(serv_addr));
    socklen_t serv_addr_len=sizeof(serv_addr);
    while(1){
        struct Packet *MyPacket = new Packet;
        valread = recvfrom(sock, buffer, sizeof (Packet),0,(struct sockaddr*)&serv_addr,&serv_addr_len);

        deserializePacket(buffer, MyPacket);
        if(strcmp(MyPacket->data,"CLS")==0){
            fclose(pFileTXT);
            break;
        }
        if (ExpectedSequenceNumber == MyPacket->seqno) {
            fwrite(MyPacket->data,MyPacket->len-6,1,pFileTXT);//TODO not only txt %c ?

            struct Ack_packet MyAckPacket = {6, ExpectedSequenceNumber};
            char array[sizeof(Ack_packet)];
            serializePacketAck(&MyAckPacket, array);
            sendto(sock, array, sizeof(array), 0,(struct sockaddr*)&serv_addr,serv_addr_len);
            ExpectedSequenceNumber+=1;
            ExpectedSequenceNumber=ExpectedSequenceNumber%64;
        } else {
            cout<<MyPacket->seqno<<endl;
            struct Ack_packet MyAckPacket = {6, (ExpectedSequenceNumber + 63) % 64};
            char array[sizeof(Ack_packet)];
            serializePacketAck(&MyAckPacket, array);
            sendto(sock, array, sizeof(array), 0,(struct sockaddr*)&serv_addr,serv_addr_len);
        }
    }
    return 0;
}
