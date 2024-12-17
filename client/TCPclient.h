#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>

class TCPclient
{
public:
    TCPclient();
    ~TCPclient();
    bool connect_server(const char *ip, unsigned short port);
    int recv_message(unsigned char *data, int len);
    int send_message(unsigned char *data, int len);
    int client_fd();

private:
    int sockfd;
};
#endif