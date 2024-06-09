#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <vector>
#include <thread>
#include "socketmethods.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

struct calculator
{
    sockaddr_in client_addr ={0};
    socklen_t addrLen = sizeof(client_addr);
};

struct Packet
{
public:
    size_t hash;
    char data[1024];
    int getlength()
    {
        return sizeof(size_t)+strlen(data);
    }
};

struct PacketInt
{
public:
    size_t hash;
    int data;
    int getlength()
    {
        return sizeof(size_t)+sizeof(int);
    }
};

struct Matrix
{
public:
    int line,column;
    double elements[1000];
    Matrix(){};
    Matrix(int line,int column,double* elements)
    {
        this->line = line;
        this->column = column;
        //his->elements = new double[line*column];
        for(int i=0;i<line;i++)
        {
            for(int j=0;j<column;j++)
            {
                this->elements[i*column + j] = elements[i*column + j];
            }
        }
    }
    void Set(int line,int column,double* elements)
    {
        //this->elements = new double[line*column];
        this->line = line;
        this->column = column;
        for(int i=0;i<line;i++)
        {
            for(int j=0;j<column;j++)
            {
                this->elements[i*column + j] = elements[i*column + j];
            }
        }
    }
    int getlength()
    {
        return (sizeof(double)*line*column)+(sizeof(int)*2);
    }

        void Print()
    {
        for(int i=0;i<line;i++)
        {
            for(int j=0;j<column;j++)
            {
                cout<<this->elements[i*column + j]<<" ";
            }
            cout<<endl;
        }
    }
};


class UdpClient
{
public:
    int serverSocket;
    struct sockaddr_in serverAddress;
    int serverport = 9050;

    int connectSocket;
    struct sockaddr_in connectAddress;
    int connectport = 20002;

    struct sockaddr_in newAddress;


public:

    UdpClient(); 

    void sendMessage();

    void SendTask(Matrix A);


    void CreateConnectSocket();
    void ConnectToServer();

    void SendServer();
    void ListenServer();
    void Start();

    json CreateJsonObject(char type,vector<int> massiv);

};

#endif // UDPCLIENT_H
