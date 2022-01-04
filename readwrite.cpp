/*
** CITATION FROM https://www.youtube.com/watch?v=7d7_G81uews for file reading and writing using sockets
** CITATION FROM https://stackoverflow.com/questions/13097375/sending-images-over-sockets for image reading and writing using sockets
** CITATION FROM https://www.cplusplus.com/reference/istream/istream/getline/ for reading file character by character
**
*/
#define NOREADPACKETS 20
#include <queue>
#include "readwrite.h"
#include "Packet.h"
#include "cwnd.h"
void copyResponseMessageHeaderToBuffer(string requestMessage,char * buffer){
    for(int i=0;i<requestMessage.length();i++){
        buffer[i]=requestMessage[i];
    }
}

int getFirstEmptyIndex(char * buffer){
    for(int i=0;i<SIZE;i++){
        if(buffer[i]=='\0'){
            return i;
        };
    }
    return -1;//Won't occur
}

int generateResponseMessageHeaderIntoBufferGET(const char filePath[],char * buffer){

  FILE * fp;
  fp=fopen(filePath,"r");
  string responseLine;
  int response;
  if(fp==NULL){
    responseLine="HTTP/1.1 404 Not Found\r\n\r\n";
    response=-1;
  }
  else {
    responseLine="HTTP/1.1 200 OK\r\n";
    response=0;
    int size;
    fseek(fp,0,SEEK_END);
    size=ftell(fp);
    fseek(fp,0,SEEK_SET);
    responseLine+="Content-Length: "+to_string(size);
    responseLine+="\r\n\r\n";
  }
  if(fp!=NULL){
    fclose(fp);
  }
  cout<<"The Generated Response"<<endl;
  cout<<responseLine<<endl;
  copyResponseMessageHeaderToBuffer(responseLine,buffer);
  return response;
}

void generateResponseMessageHeaderIntoBufferPOST(const char filePath[],char * buffer){

  string responseLine;
  responseLine="HTTP/1.1 200 OK\r\n\r\n";
  copyResponseMessageHeaderToBuffer(responseLine,buffer);
}

int copyDataToBuffer(char  buffer[],int firstEmptyIndex,char readData[],int readDataSize ){
  int index=0;
  while(readDataSize--){
    buffer[firstEmptyIndex++]=readData[index++];
  }
  return firstEmptyIndex;
}
void copyDataFromBuffer(char buffer[],int dataStartingIndex,char readData[],int readDataSize){
  int index=0;
  while(readDataSize--){
    readData[index++]=buffer[dataStartingIndex++];
  }

}




void sendFile(const char filePath[],int sockfd,char * buffer,queue<Packet*> packetsQueue,struct cwnd * cwnd){
    FILE * fp;
  
    fp=fopen(filePath,"r");
    int fileSize;
    fseek(fp,0,SEEK_END);
    fileSize=ftell(fp);
    fseek(fp,0,SEEK_SET);
    while (fileSize>0){
          int readDataSize= min(SIZE,fileSize);
          fileSize-=readDataSize;
          char readData[readDataSize];
          fread(readData,readDataSize,1,fp);
          Packet* packet=new Packet;
          strcpy(packet->data,readData);
          packet->len=readDataSize+6;
          packetsQueue.push(packet);
          if(packetsQueue.size()==NOREADPACKETS){
              while(packetsQueue.size()>0){
                  addPacketsToCwnd(packetsQueue,cwnd);
                  sendPackets(cwnd,client_sock_fd);
              }
          }

      int sentSize=copyDataToBuffer(buffer,firstEmptyIndex,readData,readDataSize);

        if((send(sockfd,buffer,sentSize,0))==-1){
          perror("Error in sending message from client to server in post request na na na na");
            exit (1);
        }
        bzero(buffer,SIZE);
        bzero(readData,readDataSize);
        firstEmptyIndex=0;
    }
   //Send the last part of the data and clear the buffer
    if(fp!=NULL){
      fclose(fp);
    }


}



void storeIntoFile(char  data[], const char filePath[],int readDataSize){
  FILE * fp;
  fp=fopen(filePath,"a");

  if(fp==NULL){
    perror("Error in creating the file");
    exit(1);
  }
    fwrite(data,readDataSize,1,fp);
    fclose(fp);
}

void sendPicture(int sockfd,const char filename[]){
//Determine the picture size
FILE * picture;
picture =fopen(filename,"r");
int size;
fseek(picture,0,SEEK_END);
size=ftell(picture);
fseek(picture,0,SEEK_SET);

//Send Picture Size
write(sockfd,(void *)&size,sizeof(size));

//Send Picture

char buffer[size];
while(!feof(picture)){
  int sentSize=fread(buffer,sizeof(buffer),1,picture);
  write(sockfd,&sentSize,sizeof(sentSize));
  bzero(buffer,sizeof(buffer));
}
fclose(picture);
}

void receivePicture(int sockfd,const char filename[]){
//receive the size of the picture
int pictureSize;
read(sockfd,&pictureSize,sizeof(int));

//receive the picture
char pictureArray[pictureSize];
read(sockfd,pictureArray,sizeof(pictureArray));
FILE * savedPicture;
savedPicture=fopen(filename,"w");
fwrite(pictureArray,sizeof(pictureArray),1,savedPicture);
fclose(savedPicture);
}
