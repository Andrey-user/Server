#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "socketmethods.h"
#include <string.h>
#include <thread>
#include <iostream>
#include "udpclient.h"

using namespace std;

int main()
{

    UdpClient client = UdpClient();
    client.ConnectToServer();
    client.Start();
    return 0;
}

