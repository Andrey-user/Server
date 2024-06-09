#include "calculator.h"

//Конструктор
Calculator::Calculator()
{
    /*
    broadcastSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int broadcastEnable = 1;
    //Setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    std::memset(&broadcastAddress, 0, sizeof(broadcastAddress));
    broadcastAddress.sin_family = AF_INET;
    broadcastAddress.sin_addr.s_addr = INADDR_BROADCAST; // Пример широковещательного адреса
    broadcastAddress.sin_port = htons(1235); // Пример порта
    Bind(broadcastSocket, (struct sockaddr*)&broadcastAddress, sizeof(broadcastAddress));

    socklen_t broadcastlen = sizeof(broadcastAddress);
    Packet packet ;
    PacketInt packetint;
    char broadcastData[1024];
    memset(broadcastData,0,sizeof(broadcastData));
    int receivedLength = recvfrom(broadcastSocket, broadcastData, sizeof(packet), 0, (struct sockaddr*)&broadcastAddress, &broadcastlen);
    memcpy(&packetint, broadcastData, receivedLength);
    memcpy(&packet.data,"Hello World!",sizeof("Hello World!"));
    packet.hash=124325;
    cout<<"data = "<<packet.data<<endl;
    cout<<"hash = "<<packet.hash<<endl;
    cout<<"len = "<<packet.getlength()<<endl;
    sendto(broadcastSocket, reinterpret_cast<const char*>(&packet), packet.getlength(), 0, (struct sockaddr*)&broadcastAddress, sizeof(broadcastAddress));

    mainSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int mainEnable = 1; 
    Setsockopt(mainSocket, SOL_SOCKET, SO_BROADCAST, &mainEnable, sizeof(mainEnable));
    std::memset(&mainAddress, 0, sizeof(mainAddress));
    mainAddress.sin_family = AF_INET;
    mainAddress.sin_addr.s_addr = broadcastAddress.sin_addr.s_addr; // Пример широковещательного адреса
    mainAddress.sin_port = htons(packetint.data); // Пример порта
    */
}

void Calculator::CreateConnectSocket()
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
void Calculator::ConnectToServer()
{
    char s[1024];
    string b;
    Packet packet;
    char sendbuf[1024];
    socklen_t connectlen = sizeof(this->connectAddress);
    hash<string> hashfunc;
    cout<<"Write message, who need send: ";
    getline(cin,b);
    strcpy(s,b.c_str());
    cout<<"s = "<<s<<endl;
    cout<<"strlen = "<<strlen(s)<<endl;

    json send_json = this->CreateJsonObject('c',nullptr);

    sendto(connectSocket, send_json.dump().c_str(), send_json.dump().size(), 0, (struct sockaddr*)&connectAddress, connectlen);
    memset(s,0,sizeof(s));
    int port;
    ssize_t recv = recvfrom(connectSocket, s, sizeof(s), 0, (struct sockaddr*)&connectAddress, &connectlen);
    
    json response = json::parse(s);

    std::memset(&newAddress, 0, sizeof(newAddress));
    newAddress.sin_family = AF_INET;
    newAddress.sin_addr.s_addr =  INADDR_BROADCAST; // Пример широковещательного адреса
    newAddress.sin_port = htons(response["port"]); // Пример порт

}



//Отправить смс-очку на сервер
void Calculator::SendToServer()
{
    char s[1024];
    string b;
    Packet packet;
    char sendbuf[1024];
    socklen_t mainlen = sizeof(this->mainAddress);
    hash<string> hashfunc;
    while(true)
    {
        memset(sendbuf,0,sizeof(sendbuf));
        memset(packet.data,0,sizeof(packet.data));
        cout<<"Введит сообщение, которое нужно отправить: ";
        getline(cin,b);
        strcpy(s,b.c_str());
        cout<<"s = "<<s<<endl;
        cout<<"strlen = "<<strlen(s)<<endl;
        memcpy(&packet.data,s,strlen(s));
        packet.hash = hashfunc(packet.data);
        sendto(this->mainSocket, reinterpret_cast<const char*>(&packet), packet.getlength(), 0, (struct sockaddr*)&mainAddress, mainlen);
    }
}

void Calculator::GetMessageFromServer()
{
    char s[1024];
    char recvbuf[2048];
    PacketString newpacket;
    string b;
    socklen_t connectlen = sizeof(this->newAddress);
    while(true)
    {
        memset(s,0,sizeof(s));
        memset(newpacket.data,0,sizeof(newpacket.data));
        ssize_t recv = recvfrom(connectSocket, s, sizeof(s), 0, (struct sockaddr*)&newAddress, &connectlen);
        
        json j = json::parse(s);
        cout<<"j = "<<j<<endl;
        vector<int> dataVector = j["data"].get<vector<int>>();
        int n = dataVector.size();
   	quickSort(dataVector, 0, n - 1);
        j["data"]=dataVector;
        
        //Тут производяться расчеты
        //И возвращение значения на базу cx
        sendto(connectSocket,j.dump().c_str(), j.dump().size(),0,(struct sockaddr*)&newAddress, connectlen);
    }
}

json Calculator::CreateJsonObject(char type = 'c',const void* buf = nullptr)
{
    json j;
    if(type == 'c')
    {
        j["type"] = "connect";
        j["name"] = "calculator";
        j["port"] = 235;
        return j;
    }
    else if(type =='d')
    {
        j["type"] = "task";
        j["data"] = {"1","2","3"};
        return j;
    }
    else
    {
        j["type"]="error";
        return j;
    }
}


// Функция для разделения вектора на две части
int partition(std::vector<int>& vec, int low, int high) {
    int pivot = vec[high]; // Выбираем последний элемент как опорный
    int i = low - 1; // Индекс меньшего элемента

    for (int j = low; j < high; ++j) {
        if (vec[j] < pivot) {
            ++i;
            std::swap(vec[i], vec[j]);
        }
    }
    std::swap(vec[i + 1], vec[high]);
    return i + 1;
}

// Функция быстрой сортировки
void quickSort(std::vector<int>& vec, int low, int high) {
    if (low < high) {
        int pi = partition(vec, low, high);

        // Рекурсивно сортируем элементы до и после разделителя
        quickSort(vec, low, pi - 1);
        quickSort(vec, pi + 1, high);
    }
}



