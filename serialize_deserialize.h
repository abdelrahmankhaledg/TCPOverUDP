//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED1_SERIALIZE_DESERIALIZE_H
#define UNTITLED1_SERIALIZE_DESERIALIZE_H

#include "Packet.h"
#include "Ack_packet.h"

void serializePacket(Packet* packet,char *data);
void serializePacketAck(Ack_packet* ackPacket,char *data);
void deserializePacket(char *data ,Packet* packet);
void deserializeAckPacket(char *data,Ack_packet *ackPacket);

#endif //UNTITLED1_SERIALIZE_DESERIALIZE_H
