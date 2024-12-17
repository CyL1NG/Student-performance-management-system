#ifndef __TEACHER_H__
#define __TEACHER_H__
#include "user.h"
#include "mysqlite3.h"
#include "tcpserver.h"

class teacher : public user
{
public:
    teacher(string i, string n, string p = "123456", string j = "", int a = 0); // 构造函数
    bool login(Mysqlite3 &sqlite3);                                             // 登录
    bool select_INFO(int fd, TCPserver &server, Mysqlite3 &sqlite3);            // 老师查询
    void edit_INFO_C(int fd, TCPserver &server, Mysqlite3 &sqlite3);            // 语文老师修改
    void edit_INFO_M(int fd, TCPserver &server, Mysqlite3 &sqlite3);            // 数学老师修改
    void edit_INFO_E(int fd, TCPserver &server, Mysqlite3 &sqlite3);            // 英语老师修改

private:
    string JobTitle; // 职称
};
#endif