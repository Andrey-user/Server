#ifndef CALCULATOR_H
#define CALCULATOR_H

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

// Функция для разделения вектора на две части
int partition(std::vector<int>& vec, int low, int high);
// Функция быстрой сортировки
void quickSort(std::vector<int>& vec, int low, int high);

class Calculator
{
private:
    int broadcastSocket;
    struct sockaddr_in broadcastAddress;

    int mainSocket;
    struct sockaddr_in mainAddress;

    int connectSocket;
    struct sockaddr_in connectAddress;
    int connectport = 20002;

    struct sockaddr_in newAddress;

public:

    Calculator();       


    void CreateConnectSocket();
    void ConnectToServer();

    void GetMessageFromServer();

    json CreateJsonObject(char type,const void* buf);

};

#endif // CALCULATOR_H
