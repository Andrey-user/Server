#include "udpserver.h"

//Конструктор
UdpServer::UdpServer(int newConnectionPort)
{
    this->CreateNewConnectionSocket(newConnectionPort);
    max_client_fd = 0;
    max_calculator_fd = 0;
}


//Создаем сокет, который будет прослушивать новые подключения
void UdpServer::CreateNewConnectionSocket(int newConnectionPort)
{
    newConnectionSocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    std::memset(&newConnectionAddress, 0, sizeof(newConnectionAddress));
    newConnectionAddress.sin_family = AF_INET;
    newConnectionAddress.sin_addr.s_addr = INADDR_ANY; // Пример широковещательного адреса
    newConnectionAddress.sin_port = htons(newConnectionPort); // Пример порта<<"
    Bind(newConnectionSocket, (struct sockaddr *) &newConnectionAddress, sizeof newConnectionAddress); 
} 

//Основной метод запускающий работу сервера
void UdpServer::Start()
{
    thread t1(&UdpServer::ServerSettings,this);
    thread t2(&UdpServer::ListenNewConnection,this);
    thread t3(&UdpServer::ListenClient,this);
    thread t4(&UdpServer::SendMessageCalculator,this);
    thread t5(&UdpServer::ListenCalculator,this);
    thread t6(&UdpServer::SendMessageClient,this);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
}


//Прослушка новых подключений, и клиентов и калькуляторов
void UdpServer::ListenNewConnection()
{
    char recvbuf[1024];
    struct sockaddr_in calculatorAddress;
    socklen_t calculatorlen = sizeof(calculatorAddress);

    while(true)
    {   
        memset(recvbuf,0,sizeof(recvbuf));
        ssize_t recv = Recvfrom(newConnectionSocket, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&calculatorAddress, &calculatorlen);
        json respons = json::parse(recvbuf);
        sockaddr_in newAddres = calculatorAddress;

        if(respons["type"]=="connect")
        {
            if (respons["name"]=="calculator")
            {
                int newcalculatorsocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                struct sockaddr_in newcalculatoraddress;
                std::memset(&newcalculatoraddress, 0, sizeof(newcalculatoraddress));
                newcalculatoraddress.sin_family  = AF_INET;
                newcalculatoraddress.sin_addr.s_addr = htonl(INADDR_ANY);
                newcalculatoraddress.sin_port = 0;

                Bind(newcalculatorsocket, (struct sockaddr *) &newcalculatoraddress, sizeof (newcalculatoraddress));
                socklen_t len = sizeof(newcalculatoraddress);
                Getsockname(newcalculatorsocket,(struct sockaddr*)&newcalculatoraddress, &len);    
                unique_lock<mutex> lock(mtx_calculator_messages);
                calccalc.push_back(std::make_tuple(newcalculatorsocket, 0, calculatorAddress));
                number_free_calcs++;
                lock.unlock();
                int port = ntohs(newcalculatoraddress.sin_port);

                json send = CreateJsonObject('c',"server",port);
                sendto(newConnectionSocket,send.dump().c_str(),send.dump().size(),0,(struct sockaddr*)&calculatorAddress,sizeof(calculatorAddress));
            
                if (port > max_calculator_fd)
                    max_calculator_fd = port;
                    
                unique_lock<mutex> lock2(mtx_calculator_messages);
                cv_calculator_messages.notify_one();
                 
            }
            else if(respons["name"]=="client")
            {
                int newclientsocket = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                struct sockaddr_in newclientaddress;
                std::memset(&newclientaddress, 0, sizeof(newclientaddress));
                newclientaddress.sin_family  = AF_INET;
                newclientaddress.sin_addr.s_addr = htonl(INADDR_ANY);
                newclientaddress.sin_port = 0;

                Bind(newclientsocket, (struct sockaddr *) &newclientaddress, sizeof (newclientaddress));
                socklen_t len = sizeof(newclientaddress);
                Getsockname(newclientsocket,(struct sockaddr*)&newclientaddress, &len);
                clients.emplace_back(newclientsocket, calculatorAddress);
                int port = ntohs(newclientaddress.sin_port);

                json send = CreateJsonObject('c',"server",port);
                sendto(newConnectionSocket,send.dump().c_str(),send.dump().size(),0,(struct sockaddr*)&calculatorAddress,sizeof(calculatorAddress));

                if (port > max_client_fd)
                    max_client_fd = port;
            }
        }
        else
        {
            json send = CreateJsonObject('n');
            sendto(newConnectionSocket,send.dump().c_str(),send.dump().size(),0,(struct sockaddr*)&calculatorAddress,sizeof(calculatorAddress));
            cout<<"NoOne connection"<<endl;
        }

    }
}


//Прослушка подключенных Клиентов
void UdpServer::ListenClient()
{
    char recvbuf[2048];
    json recvjson;
    struct sockaddr_in clientAddress1;
    socklen_t clientlen1 = sizeof(clientAddress1);
    while(true)
    {
        while(max_client_fd==0)
        {
            if(max_client_fd!=0)
            break;
        }
        timeval timeout = {1, 0};
        FD_ZERO(&client_fd);
        for(auto& element : clients)
        {
            FD_SET(element.first,&client_fd);
        }

        int socketCount = select(max_client_fd + 1, &client_fd, nullptr, nullptr, &timeout);
        if (socketCount == 0)
            continue;

        for(auto client = clients.begin(); client != clients.end();)
        {
            if(FD_ISSET(client->first,&client_fd))
            {
                memset(recvbuf, 0, sizeof(recvbuf));
                ssize_t recv = recvfrom(client->first, recvbuf, 2048, 0, (struct sockaddr*)&clientAddress1, &clientlen1);
                recvjson = json::parse(recvbuf);
                if(recvjson["type"] == "disconnect")
                {
                    close(client->first);
                    client = clients.erase(client);
                    continue; 
                }
                unique_lock<mutex> lock(mtx_calculator_messages);
                messages_to_calculators.emplace_back(client->first,recvjson);
                cv_calculator_messages.notify_one();
            }
            ++client;
        }     
    }
}


//Прослушка подключенных калькуляторов
void UdpServer::ListenCalculator()
{
    char recvbuf[2048];
    json recvjson;
    struct sockaddr_in clientAddress1;
    socklen_t clientlen1 = sizeof(clientAddress1);
    while(true)
    {
        while(max_calculator_fd==0)
        {
            if(max_calculator_fd!=0)
            break;
        }
        timeval timeout = {1, 0};
        FD_ZERO(&calculator_fd);
        for(auto& element : calccalc)
        {
            if(get<1>(element)>0)
                FD_SET(get<0>(element),&calculator_fd);
        }

        int socketCount = select(max_calculator_fd + 1, &calculator_fd, nullptr, nullptr, &timeout);
        if (socketCount == 0)
            continue;

        for(auto it = calccalc.begin(); it != calccalc.end(); )
        {
            if(FD_ISSET(get<0>(*it),&calculator_fd))
            {
                memset(recvbuf, 0, sizeof(recvbuf));
                ssize_t recv = recvfrom(get<0>(*it), recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&clientAddress1, &clientlen1);
                recvjson = json::parse(recvbuf);
                if(recvjson["type"] == "disconnect")
                {
                    unique_lock<mutex> lock(mtx_calculator_messages);
                    close(get<0>(*it));
                    it = calccalc.erase(it);
                    number_free_calcs--; 
                    continue; 
                }
                unique_lock<mutex> lock(mtx_calculator_messages);
                number_free_calcs++;
                cv_calculator_messages.notify_one();
                int number = get<1>(*it);
                get<1>(*it) = 0;
                lock.unlock();
                unique_lock<mutex> lock1(mtx_client_messages);
                messages_to_clients.emplace_back(number,recvjson);
                cv_client_messages.notify_one();
            }
            ++it;
        }

    }
}


//Отправка сообщений калькулятороам
void UdpServer::SendMessageCalculator()
{
    int sock_number;
    while(true)
    {
        unique_lock<mutex> lock(mtx_calculator_messages);
        cv_calculator_messages.wait(lock, [this] { return !messages_to_calculators.empty() && number_free_calcs>0;});
        while (!messages_to_calculators.empty() && number_free_calcs > 0) {
            json j = messages_to_calculators.front().second;
            sock_number = messages_to_calculators.front().first;
            messages_to_calculators.erase(messages_to_calculators.begin());
            lock.unlock();
            auto it = std::find_if(calccalc.begin(), calccalc.end(), [](const std::tuple<int, int, sockaddr_in>& elem) {return std::get<1>(elem) == 0;});
            get<1>(*it) = sock_number;
            sendto(get<0>(*it), j.dump().c_str(), j.dump().size(), 0, (struct sockaddr*)&(get<2>(*it)), sizeof(get<2>(*it)));
            lock.lock();
            number_free_calcs--;
        }
    }
}

//Отправка сообщений клиентам
void UdpServer::SendMessageClient()
{
    int sock_number;
    while(true)
    {
        unique_lock<mutex> lock1(mtx_client_messages);
        cv_client_messages.wait(lock1, [this] { return !messages_to_clients.empty();});
        while (!messages_to_clients.empty()) {
            json j = messages_to_clients.front().second;
            sock_number = messages_to_clients.front().first;
            messages_to_clients.erase(messages_to_clients.begin());
            lock1.unlock();
            auto element = find_if(clients.begin(), clients.end(), [sock_number](const pair<int, sockaddr_in>& client) 
            {return client.first == sock_number;});
            sendto(sock_number, j.dump().c_str(), j.dump().size(), 0, (struct sockaddr*)&(element->second), sizeof(element->second));
            lock1.lock();
        }
    }
}


//Вывод информации об активных калькуляторов
void UdpServer::CalcsInfo()
{
    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];

    cout<<endl<<endl<<"Info About Calculators"<<endl;
    for(int i=0;i<calccalc.size();i++)
    {
        getnameinfo((struct sockaddr *)&get<2>(calccalc[i]), sizeof(sockaddr_in), hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);
        cout<<"IP: "<<hoststr<<"   port: "<<portstr<<endl;
    }
    cout<<endl<<"Info About CLients"<<endl;
    for(int i=0;i<clients.size();i++)
    {
        getnameinfo((struct sockaddr *)&clients[i].second, sizeof(sockaddr_in), hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);
        cout<<"IP: "<<hoststr<<"   port: "<<portstr<<endl;
    }
    cout<<endl<<endl;

}

//Узнать состояние сервера
void UdpServer::ServerSettings()
{
    char s[1024];
    string b;
    while(true)
    {
        memset(s,0,sizeof(s));
        b="";
        cout<<"command = ";
        getline(cin,b);
        cout<<"Write command = "<<b<<endl;
        if (b=="exit")
        {
            exit(1);
        }
        else if(b=="info")
        {
            this->CalcsInfo();
            cout<<"Numer fgfg = "<<number_free_calcs<<endl<<endl;
        }
        else 
        {
            cout<<"Incorrect command"<<endl;
        }
    }
}

json UdpServer::CreateJsonObject(char type,string name,int port,const void* buf)
{
    json j;
    if(type == 'c')
    {
        j["type"] = "connected";
        j["name"] = name;
        j["port"] = port;
        return j;
    }
    else if (type == 'n')
    {
        j["type"] = "noconnected";
        return j;
    }
    else
    {
        j["type"]="error";
        return j;
    }
}