#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;
class TCPserver
{

public:
    TCPserver(const string ip, unsigned short port);               // 构造函数
    ~TCPserver();                                                  // 析构函数
    int accept_connection();                                       // 接受连接
    int send_message(int client_fd, unsigned char *data, int len); // 发送信息
    int recv_message(int client_fd, unsigned char *data, int len); // 接受信息

private:
    int sockfd;
};

#endif