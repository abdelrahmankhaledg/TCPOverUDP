//
// Created by abdelrahmankhaledg on ٣‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_SLOW_START_H
#define UNTITLED_SLOW_START_H

#include "cwnd.h"
#include <queue>
void acknowledgePacketAndIncreaseCwndSize(struct cwnd cwnd,std::queue<Packet> packetsQueue,struct Ack_packet ackPacket){
    acknowledgePacket(ackPacket,cwnd);

    for(int i=0;i<2;i++){//when a Packet is acknowledged , 2 packets are added to cwnd to be sent to the client. IF CURRENT SIZE IS LESS THAN MAX SIZE
        if(checkCorrectnessOfNextSeqno(cwnd) && !isCwndMaxSizeReached(cwnd)){
           struct Packet nextPacketToSend =packetsQueue.front();
           packetsQueue.pop();
           nextPacketToSend.seqno=getNextSeqno(&cwnd);
            addPacketToCwnd(nextPacketToSend,cwnd);
        }
        else {
            break;
        }
    }
    //TODO send packets at the end of cwnd
}

#endif //UNTITLED_SLOW_START_H
