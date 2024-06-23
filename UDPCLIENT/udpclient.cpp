#include "udpclient.h"

UdpClient::UdpClient()
{
    this->CreateConnectSocket();
}

void UdpClient::CreateConnectSocket()
{
    connectSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int mainEnable = 1; 
    Setsockopt(connectSocket, SOL_SOCKET, SO_BROADCAST,reinterpret_cast<const char*>(&mainEnable), sizeof(mainEnable));
    std::memset(&connectAddress, 0, sizeof(connectAddress));
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_addr.s_addr =  INADDR_BROADCAST;
    connectAddress.sin_port = htons(connectport);
}


//Попытка первого подклчюения к серверу
void UdpClient::ConnectToServer()
{
    char  recvbuf[1024];
    socklen_t connectlen = sizeof(this->connectAddress);
    fd_set connect_fd;
    timeval timeout;
    while(true)
    {
        cout<<"Try to connect the server"<<endl;
        FD_ZERO(&connect_fd);
        json send_json = this->CreateJsonObject('c',{0});
        sendto(connectSocket, send_json.dump().c_str(), send_json.dump().size(), 0, (struct sockaddr*)&connectAddress, connectlen);

        FD_SET(connectSocket,&connect_fd);
        timeout = {5,0};
        int SocketCount = select(connectSocket+1,&connect_fd,nullptr,nullptr,&timeout);
        if(SocketCount == 0)
        {
            cout<<"No connection"<<endl;
            continue;
        }
        break;   
    }
    memset(recvbuf,0,sizeof(recvbuf));
    ssize_t recv = recvfrom(connectSocket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&connectAddress, &connectlen);
    json response = json::parse(recvbuf);
    std::memset(&newAddress, 0, sizeof(newAddress));
    newAddress.sin_family = AF_INET;
    newAddress.sin_addr.s_addr =  INADDR_BROADCAST;
    newAddress.sin_port = htons(response["port"]);
    cout<<"Connect to server"<<endl;
    

}

//Отправка заданий серверу и получение ответа
void UdpClient::Start()
{
    char s[1024];
    char recvbuf[2048];
    vector<int> massiv;
    int k;
    int m;
    socklen_t connectlen = sizeof(this->newAddress);
    ssize_t result;
    while(true)
    {
        memset(s,0,sizeof(s));
        cout<<"writen amount of elements: ";
        cin>>k;
        massiv.clear();
        for(int i=0;i<k;i++)
        {	
        	cout<<"A["<<i<<"] : ";
        	cin>>m;	
        	massiv.push_back(m);
        }
        json send = CreateJsonObject('t',massiv);
        result = Sendto(connectSocket, send.dump().c_str(), send.dump().size(), 0, (struct sockaddr*)&newAddress, connectlen);
        
        memset(recvbuf,0,sizeof(recvbuf));
        ssize_t recv = Recvfrom(connectSocket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&newAddress, &connectlen);
        recvbuf[recv] = '\0';

        json recvjson = json::parse(recvbuf);
        cout<<endl<<"Result = "<<recvjson["data"]<<endl<<endl;    
        
    }
}


json UdpClient::CreateJsonObject(char type = 'c',vector<int> massiv = {0})
{
    json j;
    if(type == 'c')
    {
        j["type"] = "connect";
        j["name"] = "client";
        j["port"] = 235;
        return j;
    }
    else if(type =='t')
    {
        j["type"] = "task";
        j["data"] = massiv;
        return j;
    }
    else
    {
        j["type"]="error";
        return j;
    }
}
