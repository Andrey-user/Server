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
    client.CreateConnectSocket();
    client.ConnectToServer();
    client.Start();


    
    /*
    int line,column;
    double * elements;
    Matrix A = Matrix();
    Packet p = Packet();
    memcpy(&p.data,"Hello",sizeof("Hello"));
    cout<<reinterpret_cast<const char*>(&p)<<endl;
    UdpClient usock = UdpClient();
    while(true)
    {
        cout<<"Введите размерность матрицы:"<<endl;
        cout<<"line = ";cin>>line;
        cout<<"column = ";cin>>column;
        cout<<"Введите значения матрицы";
        elements = new double[line*column];
        for(int i=0;i<line;i++)
        {
            for(int j=0;j<column;j++)
            {
                cout<<"A["<<i<<"]["<<j<<"] = ";cin>>elements[i*column +j];
            }
        }
        A.Set(line,column,elements);
        //usock.sendMessage();
        usock.SendTask(A);
    }
    */


    return 0;
}

