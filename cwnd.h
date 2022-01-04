//
// Created by abdelrahmankhaledg on ٣‏/١‏/٢٠٢٢.
//

#ifndef UNTITLED_CWND_H
#define UNTITLED_CWND_H
#define MAXSEQNO 64
#define MAXCWNDSIZE MAXSEQNO-1

#include <vector>
#include <algorithm>
#include <queue>
#include <sys/socket.h>
#include <cstring>
#include "Packet.h"
#include "Ack_packet.h"
#include "serialize_deserialize.h"

//TODO functions headers in all files.

void addPacketToCwnd(struct Packet* packet,struct cwnd* cwnd);
void addPacketsToCwnd( std::queue<Packet*> packetsQueue,struct cwnd* cwnd );
void acknowledgePacketsSlowStart(struct Ack_packet ackPacket, struct cwnd* cwnd);
void acknowledgePacketsCongestionAvoidance(struct Ack_packet ackPacket, struct cwnd* cwnd);
void increaseCwndSizeSlwStart(int noAcknowledgedPackets,struct cwnd* cwnd);
int acknowledgePackets(struct Ack_packet ackPacket, struct cwnd* cwnd);
void controlFlow(struct Ack_packet ackPacket, struct cwnd* cwnd);
bool checkCorrectnessOfNextSeqno(struct cwnd cwnd);
int getNextSeqno(struct cwnd * cwnd);
bool isCwndMaxSizeReached(struct cwnd cwnd);


/*
 * Code Skeleton
 * Slow Start
 * 0-double loop : outerloop (file still has data) , inner loop (queue not empty)
 * 1-call addPacketsToCwnd
 * 2-call sendPackets
 * 3-call acknowledgePackets
 * 4-call increaseCwndSize
 * 5-repeat from step 3
 */
struct cwnd {
    int state;//0 slow start , 1 congestion avoidance , 2 fast recovery
    int ssthresh;//TODO initialize with 64kbps
    int duplicateAcknowledgment;//TODO
    int lastAckno;//TODO Initialized to -1
    double fractionIncreaseInSize;//Used in case of congestion avoidance
    int currentAvailableCwndSize;
    int nextSeqno;
    timeval timeout;
    std::vector<Packet*> packetsWindow;
};

 void addPacketToCwnd(struct Packet* packet,struct cwnd* cwnd){
     packet->seqno= getNextSeqno(cwnd);
     cwnd->packetsWindow.push_back(packet);
}

void addPacketsToCwnd( std::queue<Packet*> packetsQueue,struct cwnd* cwnd ){
    int noPacketsToBeSent=cwnd->currentAvailableCwndSize-cwnd->packetsWindow.size();
    for(int i=0;i<noPacketsToBeSent && !packetsQueue.empty();i++){
        addPacketToCwnd(packetsQueue.front(),cwnd);
        packetsQueue.pop();
    }
}

void controlFlow(struct Ack_packet ackPacket, struct cwnd* cwnd){
    //TODO change the timeout timer if the Packet is the send base
    //State is not fast recovery and a duplicate ack has been received
    if(cwnd->state!=2 && ackPacket.ackno==cwnd->lastAckno){
        cwnd->duplicateAcknowledgment++;
        if(cwnd->duplicateAcknowledgment==3){
            cwnd->state=2;//State is fast recovery
            cwnd->ssthresh=cwnd->currentAvailableCwndSize/2;
            cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+3)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+3);
            //TODO retransmit all packets in cwnd
        }
        return;
    }

    //TODO timout transition from state 2 to state 0

    if (cwnd->state==0 && ackPacket.ackno!=cwnd->lastAckno){
        acknowledgePacketsSlowStart(ackPacket,cwnd);
    }

    else if (cwnd->state==1 && ackPacket.ackno!=cwnd->lastAckno){
        acknowledgePacketsCongestionAvoidance(ackPacket,cwnd);
    }
    else if(cwnd->state==2 && ackPacket.ackno!=cwnd->lastAckno){
        cwnd->duplicateAcknowledgment=0;
        cwnd->state=1;
        cwnd->currentAvailableCwndSize=cwnd->ssthresh;
        acknowledgePacketsCongestionAvoidance(ackPacket,cwnd);
    }
    else if (cwnd->state==2 && ackPacket.ackno==cwnd->lastAckno){
        cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);
        //TODO transmit new packet
    }




    /*else if (cwnd->state==2 && cwnd->duplicateAcknowledgment==3){

    }*/

// acknowledgePackets(ackPacket,cwnd);//TODO check


}

int acknowledgePackets(struct Ack_packet ackPacket, struct cwnd* cwnd){
    cwnd->lastAckno=ackPacket.ackno;
    cwnd->duplicateAcknowledgment=0;
    //remove the acknowledged Packet from the vector

    int noAcknowledgedPackets = 0;
    while ((cwnd->packetsWindow[0]->seqno <= ackPacket.ackno)) {
        cwnd->packetsWindow.erase(cwnd->packetsWindow.begin());
        noAcknowledgedPackets++;
    }
    return noAcknowledgedPackets;
 }

 void acknowledgePacketsSlowStart(struct Ack_packet ackPacket, struct cwnd* cwnd){
     cwnd->duplicateAcknowledgment=0;
     int noAcknowledgedPackets= acknowledgePackets(ackPacket,cwnd);
     increaseCwndSizeSlwStart(noAcknowledgedPackets,cwnd);
     if(cwnd->currentAvailableCwndSize>=cwnd->ssthresh){
         cwnd->state=1;//congestion avoidance
     }
     return;
 }


 void acknowledgePacketsCongestionAvoidance(struct Ack_packet ackPacket, struct cwnd* cwnd){
    cwnd->duplicateAcknowledgment=0;
    int noAcknowledgedPackets=acknowledgePackets(ackPacket,cwnd);
    while(noAcknowledgedPackets--){
        cwnd->fractionIncreaseInSize=(1.0/cwnd->currentAvailableCwndSize);
        if(cwnd->fractionIncreaseInSize>1-0.00001){
            cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);
        }
    }
 }


 int acknowledgePacketsFastRecovery(struct Ack_packet ackPacket, struct cwnd* cwnd){
        cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);

 }

void increaseCwndSizeSlwStart(int noAcknowledgedPackets,struct cwnd* cwnd){
    int nextAvailableSize=cwnd->currentAvailableCwndSize+noAcknowledgedPackets;
    cwnd->currentAvailableCwndSize= nextAvailableSize>MAXCWNDSIZE?MAXCWNDSIZE:nextAvailableSize;
}


bool checkCorrectnessOfNextSeqno(struct cwnd cwnd){
    return cwnd.nextSeqno!=cwnd.packetsWindow.front()->seqno;
}

int getNextSeqno(struct cwnd * cwnd){
    int seqno=cwnd->nextSeqno;
    cwnd->nextSeqno=(((cwnd->nextSeqno)+1)%MAXSEQNO);
    return seqno;
}

void sendPackets(struct cwnd* cwnd,int client_sock_fd){
    cwnd->timeout.tv_sec=1;
    cwnd->timeout.tv_usec=0;
    for(int i=0;i<cwnd->packetsWindow.size();i++) {
        char serializedPacket[sizeof(Packet)];
        struct Packet *packet = cwnd->packetsWindow[i];
        serializePacket(packet,serializedPacket);
        sendto(client_sock_fd,serializedPacket, strlen(serializedPacket),0,);
    }


 }



#endif //UNTITLED_CWND_
