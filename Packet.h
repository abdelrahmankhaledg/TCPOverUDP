//
// Created by abdelrahmankhaledg on ٣‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_PACKET_H
#define UNTITLED_PACKET_H

#include <cstdint>

struct Packet{
    uint16_t len;
    uint32_t seqno;
    char data[500];
};
#endif //UNTITLED_PACKET_H
