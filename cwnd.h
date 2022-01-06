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
#include <netinet/in.h>
#include "Packet.h"
#include "Ack_packet.h"
#include "serialize_deserialize.h"

//TODO functions headers in all files.

void addPacketToCwnd(struct Packet* packet,struct cwnd* cwnd);
void addPacketsToCwnd( std::queue<Packet*>* packetsQueue,struct cwnd* cwnd );
void acknowledgePacketsSlowStart(struct Ack_packet* ackPacket, struct cwnd* cwnd);
void acknowledgePacketsCongestionAvoidance(struct Ack_packet* ackPacket, struct cwnd* cwnd);
void increaseCwndSizeSlwStart(int noAcknowledgedPackets,struct cwnd* cwnd);
int acknowledgePackets(struct Ack_packet* ackPacket, struct cwnd* cwnd);
int controlFlow(struct Ack_packet* ackPacket, struct cwnd* cwnd);
bool checkCorrectnessOfNextSeqno(struct cwnd cwnd);
int getNextSeqno(struct cwnd * cwnd);
bool isCwndMaxSizeReached(struct cwnd cwnd);
void changeStateOnTimeout(struct cwnd* cwnd);
void initializeCwnd(struct cwnd* cwnd);
void sendPackets(struct cwnd* cwnd,int server_sock_fd,sockaddr_in cliaddr);
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
    int ssthresh;
    int duplicateAcknowledgment;
    int lastAckno;
    double fractionIncreaseInSize;//Used in case of congestion avoidance
    int currentAvailableCwndSize;
    int nextSeqno;
    std::vector<Packet*> packetsWindow;
};



#endif //UNTITLED_CWND_
