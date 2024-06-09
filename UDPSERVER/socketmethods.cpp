#include "socketmethods.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

int Socket(int domain, int type, int protocol) 
{
    int res = socket(domain, type, protocol);
    if (res == -1) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) 
{
    int res = bind(sockfd, addr, addrlen);
    if (res == -1) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Listen(int sockfd, int backlog) 
{
    int res = listen(sockfd, backlog);
    if (res == -1) 
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) 
{
    int res = accept(sockfd, addr, addrlen);
    if (res == -1) 
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) 
{
    int res = connect(sockfd, addr, addrlen);
    if (res == -1) 
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Inet_pton(int af, const char *src, void *dst) 
{
    int res = inet_pton(af, src, dst);
    if (res == 0) 
    {
        std::cout<<"inet_pton failed: src does not contain a character"
            " string representing a valid network address in the specified"
            " address family\n";
        exit(EXIT_FAILURE);
    }
    if (res == -1) 
    {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

ssize_t Recvfrom(int sockfd,void* buf,ssize_t buflen,int flags,struct sockaddr* addr,socklen_t* addrlen)
{
    ssize_t res = recvfrom(sockfd, buf, buflen, flags,addr, addrlen);
    if (res == -1) 
    {
        perror("read failed");
    }
    return res;
}

ssize_t Sendto(int sockfd,const void* buf,ssize_t buflen,int flags, struct sockaddr* addr,socklen_t addrlen)
{
    ssize_t res = sendto(sockfd, buf, buflen, 0, addr,addrlen);
    if (res == -1) 
    {
        perror("send failed");
    }
    return res;
}

int Setsockopt(int sockfd,int level,int optname,const void* optval, socklen_t optlen)
{
    int res = setsockopt(sockfd, level, optname,optval, optlen);
    if (res == -1) 
    {
        perror("setsockport failed");
    }
    return res;
}

int Getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
    ssize_t res = getsockname(sockfd, addr, addrlen);

    if (res == -1)
    {
        perror("getsockname error");
    }
    return res;
}