//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED1_ACK_PACKET_H
#define UNTITLED1_ACK_PACKET_H


#include <cstdint>

struct Ack_packet{
    uint16_t len;
    uint32_t ackno;
};


#endif //UNTITLED1_ACK_PACKET_H
