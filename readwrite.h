#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
using namespace std;
#define SIZE 500

int generateResponseMessageHeaderIntoBufferGET(const char filePath[],char * buffer);
void generateResponseMessageHeaderIntoBufferPOST(const char filePath[],char * buffer);
void sendFile(const char filePath[],int sockfd,char * buffer,int response);
int getFirstEmptyIndex(char * buffer);
void sendFile(const char filename[],int sockfd);
void storeIntoFile(char  data[],const char  filePath[],int readDataSize);
void sendPicture(int sockfd,const char filename[]);
void receivePicture(int sockfd,const char filename[]);
int copyDataToBuffer(char  buffer[],int firstEmptyIndex,char readData[],int readDataSize );
void copyDataFromBuffer(char buffer[],int dataStartingIndex,char readData[],int readDataSize);