#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "calculator.h"

using namespace std;

int main() 
{

    Calculator calc = Calculator();
    calc.CreateConnectSocket();
    calc.ConnectToServer();
    calc.GetMessageFromServer();
    return 0;

}