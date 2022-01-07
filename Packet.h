//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED1_PACKET_H
#define UNTITLED1_PACKET_H

#include <cstdint>

struct Packet{
    uint16_t len;
    uint32_t seqno;
    char data[500];
};


#endif //UNTITLED1_PACKET_H
