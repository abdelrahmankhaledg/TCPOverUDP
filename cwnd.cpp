//
// Created by abdelrahmankhaledg on ٦‏/١‏/٢٠٢٢.
//
#include <iostream>
#include "cwnd.h"
using namespace std;

void addPacketToCwnd(struct Packet* packet,struct cwnd* cwnd){
    packet->seqno= getNextSeqno(cwnd);
    cwnd->packetsWindow.push_back(packet);
}

void addPacketsToCwnd( std::queue<Packet*>* packetsQueue,struct cwnd* cwnd ){
    int noPacketsToBeSent=std::min((unsigned long)cwnd->currentAvailableCwndSize,packetsQueue->size());
    for(int i=0;i<noPacketsToBeSent;i++){
        addPacketToCwnd(packetsQueue->front(),cwnd);
        packetsQueue->pop();
    }
}

int controlFlow(struct Ack_packet* ackPacket, struct cwnd* cwnd){//-1 if the packet is duplicate and 0 if the packet isn't duplicate
    //State is not fast recovery and a duplicate ack has been received
    if(cwnd->state!=2 && ackPacket->ackno==cwnd->lastAckno){
        cwnd->duplicateAcknowledgment++;
        if(cwnd->duplicateAcknowledgment==3){
            cwnd->state=2;//State is fast recovery
            cwnd->ssthresh=cwnd->currentAvailableCwndSize/2;
            cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+3)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+3);
        }
        return -1;
    }



    if (cwnd->state==0 && ackPacket->ackno!=cwnd->lastAckno){
        acknowledgePacketsSlowStart(ackPacket,cwnd);
        return 0;
    }

    else if (cwnd->state==1 && ackPacket->ackno!=cwnd->lastAckno){
        acknowledgePacketsCongestionAvoidance(ackPacket,cwnd);
        return 0;
    }
    else if(cwnd->state==2 && ackPacket->ackno!=cwnd->lastAckno){
        cwnd->duplicateAcknowledgment=0;
        cwnd->state=1;
        cwnd->currentAvailableCwndSize=cwnd->ssthresh;
        acknowledgePacketsCongestionAvoidance(ackPacket,cwnd);
        return 0;
    }
    else if (cwnd->state==2 && ackPacket->ackno==cwnd->lastAckno){
        cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);
        return -1;
    }

}

int acknowledgePackets(struct Ack_packet* ackPacket, struct cwnd* cwnd){
    cwnd->lastAckno=ackPacket->ackno;
    cwnd->duplicateAcknowledgment=0;
    //remove the acknowledged Packet from the vector

    int noAcknowledgedPackets = 0;
    while (cwnd->packetsWindow.size()!=0 &&(cwnd->packetsWindow[0]->seqno <= ackPacket->ackno)) {
        cwnd->packetsWindow.erase(cwnd->packetsWindow.begin());
        noAcknowledgedPackets++;
    }
    return noAcknowledgedPackets;
}

void acknowledgePacketsSlowStart(struct Ack_packet* ackPacket, struct cwnd* cwnd){
    cwnd->duplicateAcknowledgment=0;
    int noAcknowledgedPackets= acknowledgePackets(ackPacket,cwnd);
    increaseCwndSizeSlwStart(noAcknowledgedPackets,cwnd);
    if(cwnd->currentAvailableCwndSize>=cwnd->ssthresh){
        cwnd->state=1;//congestion avoidance
    }
}


void acknowledgePacketsCongestionAvoidance(struct Ack_packet* ackPacket, struct cwnd* cwnd){
    cwnd->duplicateAcknowledgment=0;
    int noAcknowledgedPackets=acknowledgePackets(ackPacket,cwnd);
    while(noAcknowledgedPackets--){
        cwnd->fractionIncreaseInSize=(1.0/cwnd->currentAvailableCwndSize);
        if(cwnd->fractionIncreaseInSize>1-0.00001){
            cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);
            cwnd->fractionIncreaseInSize=0;
        }
    }
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

void sendPackets(struct cwnd* cwnd,int server_sock_fd,sockaddr_in cliaddr){
    for(int i=0;i<cwnd->packetsWindow.size();i++) {
        char serializedPacket[sizeof(Packet)]={0};
        struct Packet *packet = cwnd->packetsWindow[i];
        serializePacket(packet,serializedPacket);

        struct Packet * newPacket=new Packet;
        deserializePacket(serializedPacket,newPacket);
        sendto(server_sock_fd,serializedPacket, sizeof(serializedPacket),0,(const struct sockaddr *)&cliaddr,sizeof(cliaddr));
    }
}

void changeStateOnTimeout(struct cwnd* cwnd){
    cwnd->state=0;//slow start
    cwnd->ssthresh=cwnd->currentAvailableCwndSize/2;
    cwnd->currentAvailableCwndSize=1;
    cwnd->duplicateAcknowledgment=0;
}

void initializeCwnd(struct cwnd* cwnd){
    cwnd->state=0;//slow start
    cwnd->ssthresh=20;// TODO Check
    cwnd->duplicateAcknowledgment=0;
    cwnd->lastAckno=63;//It is set to 63 to handle the case where the sequence number is 0 at the start of the sending oepration
    cwnd->fractionIncreaseInSize=0;
    cwnd->currentAvailableCwndSize=1;
    cwnd->nextSeqno=0;
}
