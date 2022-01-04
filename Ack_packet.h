//
// Created by abdelrahmankhaledg on ٣‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_ACK_PACKET_H
#define UNTITLED_ACK_PACKET_H

#include <cstdint>

struct Ack_packet{
    uint16_t len;
    uint32_t ackno;
};
#endif //UNTITLED_ACK_PACKET_H
