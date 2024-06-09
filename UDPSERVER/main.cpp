#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include "udpserver.h"
/*#include <openssl/sha.h>
#include <openssl/md5.h>*/
#include <stdio.h>
#include <stdlib.h>


using namespace std;

int main() 
{
    UdpServer serv = UdpServer();
    serv.CreateNewConnectionSocket();
    serv.Start();


    return 0;
}

