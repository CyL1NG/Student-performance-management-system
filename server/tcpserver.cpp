#include <iostream>
#include <stdio.h>
#include <sys/types.h>
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
#include "tcpserver.h"

TCPserver::~TCPserver()
{
    close(sockfd);
}

TCPserver::TCPserver(const string ip, unsigned short port)
{
    // 创建套接字文件
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("创建套接字失败:");
        return;
    }

    // 准备绑定文件描述符号和套接字地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));               // 将套接字地址清0
    addr.sin_family = AF_INET;                    // 指定为IPv4协议族
    addr.sin_port = htons(port);                  // 转换成网络字节序
    addr.sin_addr.s_addr = inet_addr(ip.c_str()); // 指定IP地址

    // 绑定文件描述符和套接字地址(IPv4协议)
    int r = bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr));
    if (r == -1)
    {
        perror("绑定失败：");
        return;
    }

    // 3.监听
    r = listen(sockfd, 5);
    if (r == -1)
    {
        perror("监听失败：");
        return;
    }
}

int TCPserver::accept_connection()
{
    // 响应客户端的连接请求
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
    if (-1 == client_fd)
    {
        perror("响应失败:");
        return -1;
    }
    printf("客户端连接成功,客户端的IP是%s,端口是%hu\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    return client_fd;
}

int TCPserver::send_message(int client_fd, unsigned char *data, int len)
{
    // 定义一个数组保存处理后的数据
    unsigned char buf[len * 2 + 3];
    buf[0] = 0xff;

    int j = 3; // 用来保存完整数据包的长度 当前是从3 开始
    int i;
    for (i = 0; i < len; i++)
    {
        if (data[i] == 0xff)
        {
            buf[j++] = 0xfd;
            buf[j++] = 0xfe;
        }
        else if (data[i] == 0xfd)
        {
            buf[j++] = 0xfd;
            buf[j++] = 0xfd;
        }
        else
        {
            buf[j++] = data[i];
        }
    }

    short *p = (short *)&buf[1];
    *p = htons(j); // 将主机字节序转换成网络字节序

    int r = write(client_fd, buf, j);
    if (r == -1)
    {
        perror("write fail");
        return -1;
    }
    return j;
}

int TCPserver::recv_message(int client_fd, unsigned char *data, int len)
{

    // 找到数据包的包头
    unsigned char x;
    int r;
    do
    {
        r = read(client_fd, &x, 1); // 每次读取1字节,直到遇到包头
        if (r == -1)
        {
            perror("read fail");
            return -1;
        }
    } while (x != 0xff);

    // 读取数据包的长度
    int message_len;
    read(client_fd, &message_len, 2);

    message_len = ntohs(message_len); // 将网络字节序转换成主机字节序
    // 保存读取到的数据包
    unsigned char buf[message_len - 3];
    r = read(client_fd, buf, message_len - 3); // 读取的数据是经过处理的，所以要还原成原始数据
    if (r == -1)
    {
        perror("读取失败：");
        return -1;
    }

    int i, j = 0;
    for (i = 0; i < message_len - 3 && j < len; i++)
    {
        if (buf[i] == 0xfd)
        {
            i++;
            if (buf[i] == 0xfe)
            {
                data[j++] = 0xff;
            }
            else if (buf[i] == 0xfd)
            {
                data[j++] = 0xfd;
            }
        }
        else
        {
            data[j++] = buf[i];
        }
    }
    return j;
}
