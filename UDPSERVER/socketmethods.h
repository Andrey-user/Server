#ifndef SPCKETMETHODS_H
#define SOCKETMETHODS_H

#include <sys/types.h>
#include <sys/socket.h>


int Socket(int domain, int type, int protocol);

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Inet_pton(int af, const char *src, void *dst);

ssize_t Recvfrom(int sockfd,void* buf,ssize_t buflen,int flags,struct sockaddr* addr,socklen_t* addrlen);

ssize_t Sendto(int socket,const void* buf,ssize_t buflen,int flags, struct sockaddr* addr,socklen_t addrlen);

int Setsockopt(int socket,int level,int optname,const void* optval, socklen_t optlen);

int Getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

#endif