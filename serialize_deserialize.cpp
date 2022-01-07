//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#include "serialize_deserialize.h"

void serializePacket(Packet* packet,char *data){
    uint16_t * p1 = (uint16_t*)data;
    *p1=packet->len;
    p1++;
    uint32_t * p3=(uint32_t*)p1;
    *p3=packet->seqno;
    p3++;

    uint8_t *p2 = (uint8_t*)p3;
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
    uint32_t * p3=(uint32_t*)p1;
    packet->seqno=*p3;p3++;
    uint8_t *p2 = (uint8_t*)p3;
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