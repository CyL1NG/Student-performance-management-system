// #define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <thread>
#include <mutex>
#include <iostream>
#include "teacher.h"
#include "student.h"
#include "mysqlite3.h"
#include "tcpserver.h"
// string 类是char* 类型

void hand_teacher_login(char *buf, int fd, TCPserver &server, Mysqlite3 &Mysqlite3);
void hand_student_login(char *buf, int fd, TCPserver &server, Mysqlite3 &Mysqlite3);

void tongxin(int fd, TCPserver &server, Mysqlite3 &Mysqlite3)
{
    char buf[100];
    int r;
    struct pollfd fds[1]; // 定义数组,一个元素对应一个被监听的文件描述符
    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLRDHUP; // 监听客户端是否发来数据 或者断开连接
    fds[0].revents = 0;
    while (1)
    {
        r = poll(fds, 2, 5000);
        if (r == -1)
        {
            // 关闭描述符
            break;
        }
        else if (r == 0)
        {
            cout << "超时...." << endl;
        }
        else // 有数据可读，或者对端断开连接了
        {
            if (fds[0].revents & POLLRDHUP)
            {
                // 客户端断开连接
                cout << "客户端断开连接" << endl;
                break;
            }
            if (fds[0].revents & POLLIN)
            {
                // 首先读取客户端信息 识别第一个消息类型是什么
                r = server.recv_message(fd, (unsigned char *)buf, 100);
                switch (buf[0])
                {
                case T_LOGIN:
                    hand_teacher_login(buf, fd, server, Mysqlite3);
                    break;
                case S_LOGIN:
                    hand_student_login(buf, fd, server, Mysqlite3);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

int main()
{
    // 创建一个数据库对象
    Mysqlite3 sqlite3_{"/mnt/hgfs/Share/CS2418/03三阶段(c++)/Project/server/1.db"};
    // 创建一个服务端对象
    TCPserver server{"172.6.1.121", 8889};
    int fd;
    while (1)
    {

        fd = server.accept_connection(); // 响应客户端的连接请求

        /*  创建子线程 与客户端进行连接
        thread t{&func}; */
        // 创建进程与连接好的客户端进行通信
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("创建进程失败:");
            return -1;
        }
        else if (pid == 0) // 子进程
        {
            //  写一个函数与客户端通信
            tongxin(fd, server, sqlite3_);
            close(fd);
            exit(0);
        }
        // wait(NULL); //不能在此wait，因为会阻塞，子进程不结束，就没办法继续响应下一个客户端的连接请求
    }

    return 0;
}

void hand_teacher_login(char *buf, int fd, TCPserver &server, Mysqlite3 &Mysqlite3)
{
    char buf1[100];
    int r;
    // 老师登录
    string id{buf + 1};
    string name{buf + 21};
    string passwd{buf + 41};
    cout << "老师工号：" << id << " 姓名" << name << " 密码" << passwd << endl;
    teacher t{id, name, passwd};
    if (t.login(Mysqlite3))
    {
        // 发送登录成功的消息给客户端
        buf1[0] = YES;
        server.send_message(fd, (unsigned char *)buf1, 1);
        // 登录成功后
        while (1)
        {
            r = server.recv_message(fd, (unsigned char *)buf, 100);
            switch (buf[0])
            {
            case T_SELECT:
                t.select_INFO(fd, server, Mysqlite3);
                break;
            case T_EDIT:
                cout << "老师修改成绩中....." << endl;
                if (id[0] == 'C')
                    t.edit_INFO_C(fd, server, Mysqlite3);
                else if (id[0] == 'M')
                    t.edit_INFO_M(fd, server, Mysqlite3);
                else if (id[0] == 'E')
                    t.edit_INFO_E(fd, server, Mysqlite3);
                break;
            case QUIT:
                cout << "退出" << endl;
                return;
            default:
                break;
            }
        }
    }
    else
    {
        // 发送登录失败的消息给客户端
        buf1[0] = NO;
        server.send_message(fd, (unsigned char *)buf1, 1);
    }
}

void hand_student_login(char *buf, int fd, TCPserver &server, Mysqlite3 &Mysqlite3)
{
    char buf1[100];
    int r;
    // 学生登录
    string id{buf + 1};
    string name{buf + 21};
    string passwd{buf + 41};
    cout << "学生学号:" << id << " 姓名" << name << " 密码" << passwd << endl;
    student s{id, name, passwd};
    if (s.login(Mysqlite3))
    {
        // 发送登录成功的消息给客户端
        buf1[0] = YES;
        server.send_message(fd, (unsigned char *)buf1, 1);
        // 登录成功以后可以查看自己的成绩
        // 学生查询
        while (1)
        {
            r = server.recv_message(fd, (unsigned char *)buf, 100);
            if (buf[0] == S_SELECT)
            {
                s.select_INFO(fd, server, Mysqlite3);
            }
            else if (buf[0] == QUIT)
            {
                cout << "退出" << endl;
                break;
            }
        }
    }
    else
    {
        // 发送登录失败的消息给客户端
        buf1[0] = NO;
        server.send_message(fd, (unsigned char *)buf1, 1);
    }
}