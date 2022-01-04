#include <string.h>
#include <iostream>
#include <string.h>
using namespace std;
#define SIZE 500
#define GET 0
#define POST 1
string CharArrayToString(char  charArray[],int startIndex);
int findStartOfData(char receivedMessage[]);
void printRequestMsg(char *msgToken);
int getRequestType(string requestLine);
string getFilePath(string requestLine);
int getFileSize(char * buffer);