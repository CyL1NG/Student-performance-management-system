#ifndef __TEACHER_H__
#define __TEACHER_H__
#include "user.h"
#include "TCPclient.h"

class teacher : public user
{
public:
    teacher(string i, string n, string p = "123456", int a = 0, string j = ""); // 构造函数
    bool login(TCPclient &client);                                              // 登录
    int show() const;                                                           // 显示
    int show_login() const;                                                     // 显示登录界面
    void select_INFO(TCPclient &client);                                        // 老师查询
    void edit_INFO_C(TCPclient &client);                                        // 老师修改
    void edit_INFO_M(TCPclient &client);                                        // 老师修改
    void edit_INFO_E(TCPclient &client);                                        // 老师修改
    bool quit(TCPclient &client);                                               // 老师退出

private:
    string JobTitle; // 职称
};
#endif