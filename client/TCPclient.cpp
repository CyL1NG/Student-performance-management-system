#include "TCPclient.h"
using std::cerr;
using std::cout;
using std::endl;
/*
    连接服务端
*/
TCPclient::TCPclient()
{
    // 1，创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        cerr << "创建套接字失败:" << endl;
        return;
    }
}

bool TCPclient::connect_server(const char *ip, unsigned short port)
{
    //  3，发起连接请求
    struct sockaddr_in addr;        // 服务端的套接字地址
    memset(&addr, 0, sizeof(addr)); // 把整个套接字地址清0
    addr.sin_family = AF_INET;      // 指定为IPV4协议族
    addr.sin_port = htons(port);    // 假设端口是6666，要转换为网络字节序

    addr.sin_addr.s_addr = inet_addr(ip); // 指定ip地址
    int r = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (-1 == r)
    {
        perror("连接失败:");
        return false;
    }

    cout << "连接成功" << endl;
    return true;
}
TCPclient::~TCPclient()
{
    close(sockfd);
}

int TCPclient::client_fd()
{
    return sockfd;
}

int TCPclient::send_message(unsigned char *data, int len)
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

    int r = write(sockfd, buf, j);
    if (r == -1)
    {
        perror("write fail");
        return -1;
    }
    return j;
}

int TCPclient::recv_message(unsigned char *data, int len)
{

    // 找到数据包的包头
    unsigned char x;
    int r;
    do
    {
        r = read(sockfd, &x, 1); // 每次读取1字节,直到遇到包头
        if (r == -1)
        {
            perror("read fail");
            return -1;
        }
    } while (x != 0xff);

    // 读取数据包的长度
    int message_len;
    read(sockfd, &message_len, 2);

    message_len = ntohs(message_len); // 将网络字节序转换成主机字节序
    // 保存读取到的数据包
    unsigned char buf[message_len - 3];
    r = read(sockfd, buf, message_len - 3); // 读取的数据是经过处理的，所以要还原成原始数据
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

/*
----------------加密代码---------------------
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;

class TCPclient {
public:
    TCPclient();
    ~TCPclient();
    bool connect_server(const char *ip, unsigned short port);
    int client_fd();
    int send_message(unsigned char *data, int len);
    int recv_message(unsigned char *data, int len);

private:
    int sockfd;

    // AES加密的密钥，通常应该更安全地存储
    unsigned char aes_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};  // 128-bit 密钥
    unsigned char iv[AES_BLOCK_SIZE] = {0x00};  // 初始化向量，应该是随机的
};

TCPclient::TCPclient() {
    // 1，创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        cerr << "创建套接字失败:" << endl;
        return;
    }
}

bool TCPclient::connect_server(const char *ip, unsigned short port) {
    // 3，发起连接请求
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int r = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (-1 == r) {
        perror("连接失败:");
        return false;
    }
    cout << "连接成功" << endl;
    return true;
}

TCPclient::~TCPclient() {
    close(sockfd);
}

int TCPclient::client_fd() {
    return sockfd;
}

int TCPclient::send_message(unsigned char *data, int len) {
    // 使用 AES 加密数据
    unsigned char encrypted_data[len + AES_BLOCK_SIZE];  // 加密后的数据
    int encrypted_len = 0;

    // 初始化 AES 加密上下文
    AES_KEY encrypt_key;
    if (AES_set_encrypt_key(aes_key, 128, &encrypt_key) < 0) {
        cerr << "AES 密钥设置失败" << endl;
        return -1;
    }

    // 填充数据到16字节对齐（AES 块大小）
    int padding = AES_BLOCK_SIZE - (len % AES_BLOCK_SIZE);
    unsigned char padded_data[len + padding];
    memcpy(padded_data, data, len);
    memset(padded_data + len, padding, padding);  // 填充数据

    // 对数据进行加密
    for (int i = 0; i < len + padding; i += AES_BLOCK_SIZE) {
        AES_cbc_encrypt(padded_data + i, encrypted_data + encrypted_len, AES_BLOCK_SIZE, &encrypt_key, iv, AES_ENCRYPT);
        encrypted_len += AES_BLOCK_SIZE;
    }

    // 发送数据包头
    unsigned char buf[encrypted_len + 3];
    buf[0] = 0xff;
    short *p = (short *)&buf[1];
    *p = htons(encrypted_len + 3);  // 包含头部的长度
    memcpy(buf + 3, encrypted_data, encrypted_len);

    // 发送加密后的数据
    int r = write(sockfd, buf, encrypted_len + 3);
    if (r == -1) {
        perror("write fail");
        return -1;
    }

    return encrypted_len + 3;  // 返回发送的总字节数
}

int TCPclient::recv_message(unsigned char *data, int len) {
    // 接收数据
    unsigned char x;
    int r;
    do {
        r = read(sockfd, &x, 1);
        if (r == -1) {
            perror("read fail");
            return -1;
        }
    } while (x != 0xff);

    // 读取数据包长度
    int message_len;
    read(sockfd, &message_len, 2);
    message_len = ntohs(message_len);

    // 获取加密的数据
    unsigned char encrypted_data[message_len - 3];
    r = read(sockfd, encrypted_data, message_len - 3);
    if (r == -1) {
        perror("读取失败：");
        return -1;
    }

    // 使用 AES 解密数据
    unsigned char decrypted_data[message_len - 3];
    int decrypted_len = 0;

    AES_KEY decrypt_key;
    if (AES_set_decrypt_key(aes_key, 128, &decrypt_key) < 0) {
        cerr << "AES 密钥设置失败" << endl;
        return -1;
    }

    for (int i = 0; i < message_len - 3; i += AES_BLOCK_SIZE) {
        AES_cbc_encrypt(encrypted_data + i, decrypted_data + decrypted_len, AES_BLOCK_SIZE, &decrypt_key, iv, AES_DECRYPT);
        decrypted_len += AES_BLOCK_SIZE;
    }

    // 去除填充
    int padding = decrypted_data[decrypted_len - 1];
    decrypted_len -= padding;

    // 将解密后的数据拷贝到输出缓冲区
    memcpy(data, decrypted_data, decrypted_len);
    return decrypted_len;
}





















*/