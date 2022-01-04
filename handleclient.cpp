#include "handleclient.h"
void * handleClient(void * thread_arg){
    struct thread_data *my_data;
    my_data = (struct thread_data *) thread_arg;
    int client_socket_fd=my_data->client_socket_fd;


    char buffer[SIZE]={0};
    int no_read_bytes;
    int requestType=-1;//request type is not yet determined
    char * filePath;
    string fpath;
    fd_set readfds;
    struct timeval timeout;
    int sret;
    int no_requests=0;
    int timeoutValue=0;
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
       


    while(1){
        no_requests++;
        
        FD_ZERO(&readfds);
        FD_SET(client_socket_fd,&readfds);
        pthread_mutex_lock(my_data->lock);
            int timeout_sec=20-2*(*(my_data->numOfConnectedClients));
            if(timeout_sec<=0){
                timeout_sec=1;
            }
            timeout.tv_sec=timeout_sec;
            timeout.tv_usec=0;
            timeoutValue=timeout_sec;
        
        pthread_mutex_unlock(my_data->lock);
        
        sret=select(client_socket_fd+1,&readfds,NULL,NULL,&timeout);
        if(sret==-1){
            perror("error occured in select");
            break;
        }
        if(sret==0){
            bzero(buffer,SIZE);
            char * message="close now";
            send(client_socket_fd,message,sizeof(message),0);
            close(my_data->client_socket_fd);
            break;
        }
        if((no_read_bytes=read((int)client_socket_fd,buffer,SIZE))<=0){//n==0 then no more data ,n=-1 error occured
            break;
        }
        //prase the received part of the message or save it to a file if it is data
        cout<<"Printing the received message"<<endl;
        //cout<<buffer<<endl;
        cout<<"Parsing the received Message"<<endl;
        if(requestType==-1){

            
            filePath=&(fpath[0]);
        }
        if(requestType==GET){
            bzero(buffer,SIZE);
            int response =generateResponseMessageHeaderIntoBufferGET(filePath,buffer);
            //Send the requested file to the client

            cout<<"Started Sending File To The Client"<<endl;
            sendFile(filePath,(int)client_socket_fd,buffer,response);
            cout<<"File Has Been Sent To Client"<<endl;
            //pthread_exit(NULL);
        }
        else if (requestType==POST){
            int fileSize=getFileSize(buffer);
            int dataStartingIndex=-1;
            //check if the buffer contains part of the file to be posted and if so store the file
                dataStartingIndex=findStartOfData(buffer);
                FILE * out;
                if(dataStartingIndex!=-1){
                    
                        int readDataSize=no_read_bytes-dataStartingIndex;
                        char readData[readDataSize];
                        copyDataFromBuffer(buffer,dataStartingIndex,readData,readDataSize);
                        fileSize-=readDataSize;
                        out=fopen(filePath,"w");
                        fwrite(readData,readDataSize,1,out);
                        bzero(readData,readDataSize);
                        bzero(buffer,SIZE);
                    
                }
                //receive new data and check if it contains data or not and if it contains data then store the data
                    while(fileSize>0){
                      
                        if((no_read_bytes=recv((int)client_socket_fd,buffer,SIZE,0))<=0){
                            break;
                        }
                        


                        
                        if(dataStartingIndex!=-1){
                            int readDataSize=no_read_bytes;
                            char readData[readDataSize];
                            copyDataFromBuffer(buffer,0,readData,readDataSize);
                            fileSize-=readDataSize;
                            fwrite(readData,readDataSize,1,out);
                            bzero(readData,readDataSize);
                            bzero(buffer,SIZE);
                        }
                        else {
                            dataStartingIndex=findStartOfData(buffer);
                            int readDataSize=no_read_bytes-dataStartingIndex;
                            char readData[readDataSize];
                            copyDataFromBuffer(buffer,dataStartingIndex,readData,readDataSize);
                            fileSize-=readDataSize;
                            fwrite(readData,readDataSize,1,out);
                            bzero(readData,readDataSize);
                            bzero(buffer,SIZE);
                        }
                    }
                    bzero(buffer,SIZE);
                    fclose(out);
                    generateResponseMessageHeaderIntoBufferPOST(filePath,buffer);
                    //send the ok response to the client
                    if(send((int)client_socket_fd,buffer,strlen(buffer),0)==-1){
                        perror("Error in sending response line in post from server to client");
                        exit(1);
                    }

        }
        requestType=-1;
        
    }

    no_requests--;
    if(no_requests!=0){
        
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        //Open Log File
        FILE * fp=fopen("log","a");
        fprintf(fp,"Number Of requests is %d\t",no_requests);
        fprintf(fp,"Time to serve requests is %ld microseconds\n",(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())-timeoutValue*1000000);
        fclose(fp);
    }   
    cout<<"Timeout interval "<<timeoutValue<<endl;
    pthread_mutex_lock(my_data->lock);
        *(my_data->numOfConnectedClients)=*(my_data->numOfConnectedClients)-1;
    pthread_mutex_unlock(my_data->lock);
    pthread_exit(NULL);
}
