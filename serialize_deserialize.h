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


#endif //UNTITLED_SERIALIZE_DESERIALIZE_H
