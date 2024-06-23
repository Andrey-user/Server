#include "calculator.h"

//Конструктор
Calculator::Calculator()
{
    this->CreateConnectSocket();
}

void Calculator::CreateConnectSocket()
{
    connectSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int mainEnable = 1; 
    Setsockopt(connectSocket, SOL_SOCKET, SO_BROADCAST,reinterpret_cast<const char*>(&mainEnable), sizeof(mainEnable));
    std::memset(&connectAddress, 0, sizeof(connectAddress));
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_addr.s_addr =  INADDR_BROADCAST;
    connectAddress.sin_port = htons(connectport);
}


//Подклчюение к серверу
void Calculator::ConnectToServer()
{
    char  recvbuf[1024];
    socklen_t connectlen = sizeof(this->connectAddress);
    fd_set connect_fd;
    timeval timeout;
    while(true)
    {
        cout<<"Try to connect the server"<<endl;
        FD_ZERO(&connect_fd);
        json send_json = this->CreateJsonObject('c',nullptr);
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



void Calculator::GetMessageFromServer()
{
    char s[1024];
    char recvbuf[2048];
    string b;
    socklen_t connectlen = sizeof(this->newAddress);
    while(true)
    {
        memset(s,0,sizeof(s));
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



