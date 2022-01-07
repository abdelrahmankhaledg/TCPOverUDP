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
        cwnd->duplicateAcknowledgment++;
        return -1;
    }

}

int acknowledgePackets(struct Ack_packet* ackPacket, struct cwnd* cwnd){
    cwnd->lastAckno=ackPacket->ackno;
    cwnd->duplicateAcknowledgment=0;
    //remove the acknowledged Packet from the vector

    int noAcknowledgedPackets = 0;
    //int lastAck=-1;
    int size=cwnd->packetsWindow.size();
    int index=-1;
    for(int i=size-1;i>=0;i--){
        if(cwnd->packetsWindow[i]->seqno == ackPacket->ackno){
            index=i;
            break;
        }
    }
    for(int i=0;i<=index;i++){
        cwnd->packetsWindow.erase(cwnd->packetsWindow.begin());
        noAcknowledgedPackets++;
    }
    /*while (cwnd->packetsWindow.size()!=0 &&(cwnd->packetsWindow.back()->seqno == ackPacket->ackno)) {
        //lastAck=cwnd->packetsWindow[0]->seqno;
        cwnd->packetsWindow.erase(cwnd->packetsWindow.end()-1);
        noAcknowledgedPackets++;
        //if(lastAck ==63){
            //break;
        //}
    }*/
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
    //cout<<"The fraction is "<<cwnd->fractionIncreaseInSize<<endl;
    //cout<<"The size is "<<cwnd->currentAvailableCwndSize<<endl;
    /*while(noAcknowledgedPackets--){
        cwnd->fractionIncreaseInSize+=(1.0/cwnd->currentAvailableCwndSize);
        cout<<"The fraction is "<<cwnd->fractionIncreaseInSize<<endl;
        cout<<"The size is "<<cwnd->currentAvailableCwndSize<<endl;
        if(cwnd->fractionIncreaseInSize>(1-0.001)){
            cwnd->currentAvailableCwndSize=(cwnd->currentAvailableCwndSize+1)>MAXCWNDSIZE?MAXCWNDSIZE:(cwnd->currentAvailableCwndSize+1);
            cwnd->fractionIncreaseInSize=0;
        }
    }*/
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
    //cout<<"The seqno is "<<seqno<<endl;
    cwnd->nextSeqno=(((cwnd->nextSeqno)+1)%MAXSEQNO);
    return seqno;
}

void sendPackets(struct cwnd* cwnd,int server_sock_fd,sockaddr_in cliaddr){

    for(int i=0;i<cwnd->packetsWindow.size();i++) {
        //srand(40);
        int random=rand()%100;
        if(random <100) {
            char serializedPacket[sizeof(Packet)] = {0};
            struct Packet *packet = cwnd->packetsWindow[i];

            serializePacket(packet, serializedPacket);
            sendto(server_sock_fd, serializedPacket, sizeof(serializedPacket), 0, (const struct sockaddr *) &cliaddr,
                   sizeof(cliaddr));
        }



    }
}
void writePackets(struct cwnd* cwnd,int server_sock_fd,sockaddr_in cliaddr){
    for(int i=0;i<cwnd->packetsWindow.size();i++) {
        char serializedPacket[sizeof(Packet)]={0};
        struct Packet *packet = cwnd->packetsWindow[i];
        serializePacket(packet,serializedPacket);

        struct Packet * MyPacket=new Packet;
        deserializePacket(serializedPacket,MyPacket);
        FILE *pFileTXT;
        pFileTXT = fopen("/home/abdelrahmankhaledg/CLionProjects/untitled/output", "a");
        fwrite(MyPacket->data,MyPacket->len-6,1,pFileTXT);
        fclose(pFileTXT);
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
    cwnd->ssthresh=40;// TODO Check
    cwnd->duplicateAcknowledgment=0;
    cwnd->lastAckno=63;//It is set to 63 to handle the case where the sequence number is 0 at the start of the sending oepration
    cwnd->fractionIncreaseInSize=0;
    cwnd->currentAvailableCwndSize=1;
    cwnd->nextSeqno=0;
}

