//
// Created by abdelrahmankhaledg on ٤‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_SERIALIZE_DESERIALIZE_H
#define UNTITLED_SERIALIZE_DESERIALIZE_H

#include "Packet.h"
#include "Ack_packet.h"

void serializePacket(Packet* packet,char *data);
void serializePacketAck(Ack_packet* ackPacket,char *data);
void deserializePacket(char *data ,Packet* packet);
void deserializeAckPacket(char *data,Ack_packet *ackPacket);

void serializePacket(Packet* packet,char *data){
    uint16_t * p1 = (uint16_t*)data;
    *p1=packet->len;
    p1++;
    *p1=packet->seqno;
    p1++;

    char *p2 = (char*)p1;
    int  i = 0;
    while(i<500){
        *p2=packet->data[i];
        p2++;
        i++;
    }

}

void serializePacketAck(Ack_packet* ackPacket,char *data){
    uint16_t * p1 = (uint16_t*)data;
    *p1=ackPacket->len;
    p1++;
    *p1=ackPacket->ackno;
    p1++;
}
void deserializePacket(char *data ,Packet* packet){
    uint16_t *p1 =(uint16_t*)data;
    packet->len=*p1;p1++;
    packet->seqno=*p1;p1++;
    char *p2 = (char*)p1;
    int  i = 0 ;
    while(i<500){
        packet->data[i]=*p2;
        p2++;
        i++;
    }
}


void deserializeAckPacket(char *data,Ack_packet *ackPacket){
    uint16_t *p1 =(uint16_t*)data;
    ackPacket->len=*p1;
    p1++;
    ackPacket->ackno=*p1;
}
#endif //UNTITLED_SERIALIZE_DESERIALIZE_H
