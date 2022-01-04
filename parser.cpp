#include "parser.h"


string CharArrayToString(char  charArray[],int startIndex){
    string output="";
    for(int i=startIndex;i<SIZE && charArray[i]!='\0';i++){
        output=output+charArray[i];
    }
    return output;
}

int findStartOfData(char  receivedMessage[]){
    string bufferString=CharArrayToString(receivedMessage,0);
    string delimiter="\r\n";
    int start=0;
    int end=bufferString.find(delimiter);
    while(end!=-1){
        //Empty line found
        if(bufferString.substr(start,end-start).empty()){
            start=end+delimiter.size();
            return start;
        }
        start=end+delimiter.size();
        end=bufferString.find(delimiter,start);
    }
    return -1;
}

void printRequestMsg(char *msgToken){//print the get request or the post request 
  const char delimiter[]="\r\n";
  char * token=msgToken;
  while(token!=NULL){
      cout<<token<<endl;
      token=strtok(NULL,delimiter);
  }
}

int getRequestType(string requestLine){//return the type of the msg : 0 for GET , 1 for POST
    string delimiter=" ";
    int start=0;
    int end=requestLine.find(delimiter);
    string methodName =requestLine.substr(start,end-start);
    if(methodName.compare("GET")==0){
    return GET;
    }
    else{
    return POST;
    }
}

string getFilePath(string requestLine){
    string delimiter=" ";
    int start=requestLine.find(delimiter)+delimiter.size();
    int end=requestLine.find(delimiter,start);
    return requestLine.substr(start,end-start);
}

int getFileSize(char * buffer){
 int fileSize=-1;
string message=CharArrayToString(buffer,0);
string delimiter="\r\n";
int start=message.find(delimiter)+delimiter.size();
int end=message.find(delimiter,start);
    //Find the length of the requested file
    while(end!=-1){
        string line=message.substr(start,end-start);
        string lineDelim=" ";
        int lineStart=0;
        int lineEnd=line.find(lineDelim);
        string headerFieldName=line.substr(lineStart,lineEnd-lineStart);
        if((headerFieldName.compare("Content-Length:"))==0){
            lineStart=lineEnd+lineDelim.size();
            lineEnd=line.find(lineDelim,lineStart);
            fileSize=stoi(line.substr(lineStart,lineEnd-lineStart));
            break;
        }
        start=end+delimiter.size();
        end=message.find(delimiter,start);
    }
return fileSize;
}
