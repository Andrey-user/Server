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


class UdpClient
{
public:

    int connectSocket;
    struct sockaddr_in connectAddress;
    int connectport = 20002;

    struct sockaddr_in newAddress;

public:

    UdpClient(); 

    void CreateConnectSocket();
    void ConnectToServer();
    void Start();

    json CreateJsonObject(char type,vector<int> massiv);

};

#endif // UDPCLIENT_H
