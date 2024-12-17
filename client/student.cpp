#include "student.h"
#include "TCPclient.h"
#include "user.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <string.h>
using std::vector;
student::student(string i, string n, string p, int a, string classid, string CID, string MID, string EID, double c, double m, double e) : user{i, n, a, p}
{
    // score = s;

    class_ID = classid; // 班级号
    China_ID = CID;     // 语文老师工号
    China_score = c;    // 语文成绩
    Math_ID = MID;      // 数学老师工号
    Math_score = m;     // 数学成绩
    English_ID = EID;   // 英语老师工号
    English_score = e;  // 英语成绩
}

bool student::login_S(TCPclient &client)
{

    unsigned char buf[61];
    buf[0] = S_LOGIN; // 登录的消息类型
    // 消息类型(1字节) ID(20字节) 姓名(20字节) 密码(20字节)
    memset(buf + 1, 0, 60);
    strncpy((char *)buf + 1, id.c_str(), 20);
    strncpy((char *)buf + 21, name.c_str(), 20);
    strncpy((char *)buf + 41, passwd.c_str(), 20);
    int r = client.send_message(buf, 61); // 将消息发送给服务端
    // cout << "发送r=" << r << "个数据" << endl;

    // 接受服务端发送的信息 登录成功YES or 登录失败NO
    r = client.recv_message(buf, 100);
    if (buf[0] == YES)
    {
        return true;
    }
    else if (buf[0] == NO)
    {
        return false;
    }
}

int student::show_login() const
{
    int x;
    while (true) // 使用 true 替代 1，提升可读性
    {
        cout << endl; // 在每次循环前打印一个空行，增加可读性
        cout << "===============================" << endl;
        cout << "        登录系统              " << endl; // 更具描述性的标题
        cout << "===============================" << endl;
        cout << "      输入 4 进行登录" << endl; // 更清晰的提示
        cout << "      输入 0 退出" << endl;
        cout << "===============================" << endl;
        cout << "请输入你的选择: "; // 提示用户输入选择

        cin >> x;
        if (x == 4 || x == 0)
            return x;
        else
            cout << "输入有误，请重新输入" << endl; // 纠正中文标点，增强可读性
    }
}

int student::show_INFO() const
{
    int x;
    while (true) // 使用 true 使循环更清晰
    {
        cout << endl; // 每次循环前打印一个空行，增加可读性
        cout << "===============================" << endl;
        cout << "         查看信息               " << endl; // 更具描述性的标题
        cout << "===============================" << endl;
        cout << "      输入 5 查看信息" << endl; // 清晰的操作提示
        cout << "      功能待开发..." << endl;   // 说明功能正在开发
        cout << "      输入 0 退出" << endl;     // 提供退出选项
        cout << "===============================" << endl;
        cout << "请输入你的选择: "; // 提示用户输入选择

        cin >> x; // 获取用户输入
        if (x == 5 || x == 0)
            return x; // 返回有效选择
        else
            cout << "输入有误，请重新输入" << endl; // 提供反馈信息
    }
}

void student::select_INFO(TCPclient &client) // 学生查询
{
    unsigned char buf[1000];
    buf[0] = S_SELECT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    vector<vector<string>> v;              // 存储接收到的学生信息
    // 接受服务端发送的学生信息表的信息
    while (1)
    {
        r = client.recv_message(buf, 1000); // 接收数据
        if (r <= 0)
        { // 如果没有接收到数据或出现错误，退出循环
            cout << "没接收到信息" << endl;
            break;
        }
        buf[r] = '\0'; // 确保字符串结束

        // 检查是否接收到结束标识符
        if (strcmp((const char *)(buf), "END_OF_DATA") == 0)
        {
            // cout << "接收到结束标识符，停止接收数据。" << endl;
            break;
        }

        // 处理接收到的数据，按行分割
        char *token = strtok((char *)(buf), "\n"); // 按行分割
        while (token != nullptr)
        {
            vector<string> studentInfo; // 存储每个学生的信息
            stringstream ss(token);     // 将行字符串转换为字符串流
            string item;

            // 按空格分割信息
            while (ss >> item)
            {
                studentInfo.push_back(item);
            }
            v.push_back(studentInfo);      // 将该学生信息加入容器
            token = strtok(nullptr, "\n"); // 获取下一行
        }
    }

    // 输出接收到的学生信息
    cout << "============================学生信息:=================================================================\n";
    cout << " 班级     学号      姓名   年龄    CID     语文成绩     MID       数学成绩     EID      英语成绩   密码\n";
    for (const auto &student : v)
    {
        for (const auto &info : student)
        {
            cout << info << "   "; // 输出每个信息
        }
        cout << endl; // 换行
    }
}

bool student::quit_S(TCPclient &client)
{
    unsigned char buf[1000];
    buf[0] = QUIT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    cout << "发送退出消息" << endl;
}