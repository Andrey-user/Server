#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <vector>
#include <thread>
#include "socketmethods.h"
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

struct calculator
{
    sockaddr_in client_addr ={0};
    socklen_t addrLen = sizeof(client_addr);
};



struct PacketString
{
private:
    int packettype = 1;
public:
    char data[1024];
    int getlength()
    {
        return sizeof(packettype) + strlen(data);
    }
};


struct PacketInfo
{
private:
    int packettype = 0;
public:
    int socket_number;
    int info;
    int getlength()
    {
        return sizeof(packettype) + sizeof(socket_number) + sizeof(info);
    }
    int get_type()
    {
        return packettype;
    }
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




class UdpServer
{
public:
    //Буфер который хранит сообщения, которые надо отправить калькуляторам
    vector<pair<int, json>> messages_to_calculators;
    vector<pair<int, json>> messages_to_clients;


    std::mutex mtx_calculator_messages;
    std::condition_variable cv_calculator_messages;


    std::mutex mtx_client_messages;
    std::condition_variable cv_client_messages;



    //Хранятся подключенные к серверу калькуляторы и клиенты
    vector<pair<int, sockaddr_in>> calcs;
    vector<pair<int, sockaddr_in>> clients;

    //  (номер сокета)(информация  о калькуляторе: 0 - свободен, 1 - обрабатывает запрос)(адрес калькулятора)
    vector<tuple<int,int,sockaddr_in>> calccalc;
    //количество свободных калькуляторов
    int number_free_calcs;

    vector<int> newclients;
    vector<int> newcalculators;

    int newConnectionSocket;
    struct sockaddr_in newConnectionAddress;
    int newConnectionPort = 20002;

    fd_set client_fd;
    int max_client_fd;
    int max_calculator_fd;

    fd_set calculator_fd;



public:

    UdpServer(); 
    void CalcsInfo();

    void ListenNewConnection();
    void CreateNewConnectionSocket();

    void SendMessageCalculator();
    void SendMessageClient();

    void ServerSettings();

    void Start();
    void SendAllClients();


    void ListenClient();
    void ListenCalculator();

    json CreateJsonObject(char type = 'c',string name="server",int port=0,const void* buf = nullptr);

};

#endif // UDPSERVER_H
