#include "udpclient.h"

UdpClient::UdpClient()
{
    cout<<"Start"<<endl;
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverport); // Пример порта
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
    cout<<"end"<<endl;
}

void UdpClient::sendMessage()
{
    char buf[1024] = "Hello World";
    socklen_t serverlen = sizeof(serverAddress);
    cout<<"serverSocket = "<<serverSocket<<endl;
    cout<<"LEn = "<<strlen(buf)<<endl;
    sendto(serverSocket, buf, strlen(buf), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    cout<<"Otpravili"<<endl;
}

void UdpClient::SendTask(Matrix A)
{
    socklen_t serverlen = sizeof(serverAddress);
    cout<<"serverSocket = "<<serverSocket<<endl;
    ssize_t send = sendto(serverSocket, reinterpret_cast<const char*>(&A) , A.getlength(), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    cout<<"Send = "<<send<<endl;
}

void UdpClient::CreateConnectSocket()
{
    connectSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int mainEnable = 1; 
    Setsockopt(connectSocket, SOL_SOCKET, SO_BROADCAST,reinterpret_cast<const char*>(&mainEnable), sizeof(mainEnable));
    std::memset(&connectAddress, 0, sizeof(connectAddress));
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_addr.s_addr =  INADDR_BROADCAST; // Пример широковещательного адреса
    connectAddress.sin_port = htons(connectport); // Пример порт
}


//Попытка первого подклчюения к серверу
void UdpClient::ConnectToServer()
{
    char s[1024];
    string b;
    char sendbuf[1024];
    socklen_t connectlen = sizeof(this->connectAddress);
    hash<string> hashfunc;
    cout<<"Write message, who need send: ";
    getline(cin,b);
    strcpy(s,b.c_str());
    cout<<"s = "<<s<<endl;
    cout<<"strlen = "<<strlen(s)<<endl;


    // Создание JSON-объекта для отправки
    json send_json = this->CreateJsonObject('c',{0});

    // Сериализация JSON-объекта и отправка на сервер
    string send_data = send_json.dump();
    ssize_t result = Sendto(connectSocket, send_json.dump().c_str(), send_json.dump().size(), 0, (struct sockaddr*)&connectAddress, connectlen);

    //Получение данных с сервера
    memset(s,0,sizeof(s));
    ssize_t recv = Recvfrom(connectSocket, s, sizeof(s), 0, (struct sockaddr*)&connectAddress, &connectlen);
    json response = json::parse(s);
    
    //Создание нового подключения
    std::memset(&newAddress, 0, sizeof(newAddress));
    newAddress.sin_family = AF_INET;
    newAddress.sin_addr.s_addr =  INADDR_BROADCAST; // Пример широковещательного адреса
    newAddress.sin_port = htons(response["port"]); // Пример порт
    

}

void UdpClient::Start()
{
    thread t1(&UdpClient::SendServer,this);
    //thread t2(&UdpClient::ListenServer,this);
    t1.join();
    //t2.join();
}

void UdpClient::SendServer()
{
    char s[1024];
    char recvbuf[2048];
    vector<int> massiv;
    string b;
    int k;
    int m;
    socklen_t connectlen = sizeof(this->newAddress);
    ssize_t result;
    while(true)
    {
        memset(s,0,sizeof(s));
        b="";
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
        cout<<"json_send = "<<send.dump()<<endl;
        result = Sendto(connectSocket, send.dump().c_str(), send.dump().size(), 0, (struct sockaddr*)&newAddress, connectlen);
        cout<<"result = "<<result<<endl;
        if(b=="disconnect")
            return;
        ssize_t recv = Recvfrom(connectSocket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&newAddress, &connectlen);
        cout<<endl<<endl<<"Result = "<<recvbuf<<endl<<endl;    
        
    }
}

void UdpClient::ListenServer()
{
    char recvbuf[1024];
    socklen_t connectlen = sizeof(this->newAddress);
    while(true)
    {
        ssize_t recv = Recvfrom(connectSocket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&newAddress, &connectlen);
        cout<<endl<<endl<<"Result = "<<recvbuf<<endl<<endl;
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
    else if(type =='d')
    {
        j["type"] = "task";
        j["data"] = {"1","2","3"};
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
