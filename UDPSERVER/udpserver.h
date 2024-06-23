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


class UdpServer
{
public:
    //Буфер который хранит сообщения, которые надо отправить калькуляторам
    vector<pair<int, json>> messages_to_calculators;
    vector<pair<int, json>> messages_to_clients;

    //Переменные для блокировки буфера
    std::mutex mtx_calculator_messages;
    std::condition_variable cv_calculator_messages;
    std::mutex mtx_client_messages;
    std::condition_variable cv_client_messages;


    //Хранятся подключенные к серверу калькуляторы и клиенты
    vector<pair<int, sockaddr_in>> clients;
    //  (номер сокета)(информация  о калькуляторе: 0 - свободен, номер сокета - обрабатывает запрос)(адрес калькулятора)
    vector<tuple<int,int,sockaddr_in>> calccalc;
    //количество свободных калькуляторов
    int number_free_calcs;

    //Сокет прослушивающий новые подклечения(клиенты и калькуляторы)
    int newConnectionSocket;
    struct sockaddr_in newConnectionAddress;

    //Переменные для select
    fd_set client_fd;
    fd_set calculator_fd;
    //Максимальный дескриптор клиента и калькулятора
    int max_client_fd;
    int max_calculator_fd;

public:

    UdpServer(int newConnectionPort); 
    void CalcsInfo();

    void ListenNewConnection();
    void CreateNewConnectionSocket(int newConnectionPort);

    void SendMessageCalculator();
    void SendMessageClient();

    void ListenClient();
    void ListenCalculator();

    void ServerSettings();

    void Start();

    json CreateJsonObject(char type = 'c',string name="server",int port=0,const void* buf = nullptr);

};

#endif // UDPSERVER_H
