#include "teacher.h"
#include "TCPclient.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <string.h>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::stringstream;
using std::vector;
teacher::teacher(string i, string n, string p, int a, string j) : user{i, n, a, p}
{
    JobTitle = j;
}

bool teacher::login(TCPclient &client)
{

    unsigned char buf[61];
    buf[0] = T_LOGIN; // 登录的消息类型
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

int teacher::show() const
{
    int x;
    while (true) // 使用 true 使循环更清晰
    {
        cout << endl; // 每次循环前打印一个空行，增加可读性
        cout << "===============================" << endl;
        cout << "         教师管理菜单          " << endl; // 更具描述性的标题
        cout << "===============================" << endl;
        cout << "      输入 2 查询学生信息" << endl; // 清晰的操作提示
        cout << "      输入 3 修改学生信息" << endl; // 清晰的操作提示
        cout << "      输入 0 退出" << endl;         // 提供退出选项
        cout << "===============================" << endl;
        cout << "请输入你的选择: "; // 提示用户输入选择

        cin >> x; // 获取用户输入
        if (x == 2 || x == 3 || x == 0)
            return x; // 返回有效选择
        else
            cout << "输入有误，请重新输入" << endl; // 提供反馈信息
    }
}

int teacher::show_login() const
{
    int x;
    while (true) // 使用 true 使循环更清晰
    {
        cout << endl; // 每次循环前打印一个空行，增加可读性
        cout << "===============================" << endl;
        cout << "          教师登录菜单         " << endl; // 更具描述性的标题
        cout << "===============================" << endl;
        cout << "          输入 1 登录               " << endl; // 清晰的操作提示
        cout << "          输入 0 退出               " << endl; // 提供退出选项
        cout << "===============================" << endl;
        cout << "请输入你的选择: "; // 提示用户输入选择

        cin >> x; // 获取用户输入
        if (x == 1 || x == 0)
            return x; // 返回有效选择
        else
            cout << "输入有误，请重新输入" << endl; // 提供反馈信息
    }
}

void teacher::select_INFO(TCPclient &client) // 老师查询
{
    unsigned char buf[1000];
    buf[0] = T_SELECT;
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
    // 班级 学号 姓名 年龄 语文老师工号 语文成绩 数学老师工号 数学成绩 英语老师工号 英语成绩 密码
    //  输出接收到的学生信息
    cout << "============================学生信息:=================================================================\n";
    cout << " 班级     学号      姓名   年龄    CID     语文成绩     MID       数学成绩     EID      英语成绩    密码\n";
    for (const auto &student : v)
    {
        for (const auto &info : student)
        {
            cout << info << "   "; // 输出每个信息
        }
        cout << endl; // 换行
    }
}

void teacher::edit_INFO_C(TCPclient &client)
{
    unsigned char buf[1000];
    vector<vector<string>> v;
    buf[0] = T_EDIT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    // 输入要修改的学生学号
    cout << "请输入你要修改的学生学号信息：";
    // string id;
    // cin >> id;
    // cout << id;
    scanf("%s", buf);
    // puts((char *)buf);
    // cout << "输入完成" << endl;
    //  发送要修改的学生ID给服务端
    r = client.send_message(buf, 100);
    if (r == -1)
    {
        cout << "发送学生ID失败" << endl;

        return;
    }
    // cout << "发送成功" << endl;

    // 接收服务端查找出来的学生信息
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
    cout << " 班级     学号      姓名   年龄    CID     语文成绩     MID       数学成绩     EID      英语成绩    密码\n";
    for (const auto &student : v)
    {
        for (const auto &info : student)
        {
            cout << info << "   "; // 输出每个信息
        }
        cout << endl; // 换行
    }

    // 输入你要修改的成绩
    cout << "请输入你要修改的学生语文成绩：";
    string score;
    cin >> score;
    client.send_message((unsigned char *)score.c_str(), score.length());
}

void teacher::edit_INFO_M(TCPclient &client)
{
    unsigned char buf[1000];
    vector<vector<string>> v;
    buf[0] = T_EDIT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    // 输入要修改的学生学号
    cout << "请输入你要修改的学生学号信息：";
    // string id;
    // cin >> id;
    // cout << id;
    scanf("%s", buf);
    // puts((char *)buf);
    // cout << "输入完成" << endl;
    //  发送要修改的学生ID给服务端
    r = client.send_message(buf, 100);
    if (r == -1)
    {
        cout << "发送学生ID失败" << endl;
        return;
    }
    // cout << "发送成功" << endl;

    // 接收服务端查找出来的学生信息
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
    cout << " 班级     学号      姓名   年龄    CID     语文成绩     MID       数学成绩     EID      英语成绩    密码\n";
    for (const auto &student : v)
    {
        for (const auto &info : student)
        {
            cout << info << "   "; // 输出每个信息
        }
        cout << endl; // 换行
    }

    // 输入你要修改的成绩
    cout << "请输入你要修改的学生数学成绩：";
    string score;
    cin >> score;
    client.send_message((unsigned char *)score.c_str(), score.length());
}

void teacher::edit_INFO_E(TCPclient &client)
{
    unsigned char buf[1000];
    vector<vector<string>> v;
    buf[0] = T_EDIT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    // 输入要修改的学生学号
    cout << "请输入你要修改的学生学号信息：";
    // string id;
    // cin >> id;
    // cout << id;
    scanf("%s", buf);
    // puts((char *)buf);
    // cout << "输入完成" << endl;
    //  发送要修改的学生ID给服务端
    r = client.send_message(buf, 100);
    if (r == -1)
    {
        cout << "发送学生ID失败" << endl;
        return;
    }
    // cout << "发送成功" << endl;

    // 接收服务端查找出来的学生信息
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
    cout << " 班级     学号      姓名   年龄    CID     语文成绩     MID       数学成绩     EID      英语成绩    密码\n";
    for (const auto &student : v)
    {
        for (const auto &info : student)
        {
            cout << info << "   "; // 输出每个信息
        }
        cout << endl; // 换行
    }

    // 输入你要修改的成绩
    cout << "请输入你要修改的学生英语成绩：";
    string score;
    cin >> score;
    client.send_message((unsigned char *)score.c_str(), score.length());
}
bool teacher::quit(TCPclient &client)
{
    unsigned char buf[1000];
    buf[0] = QUIT;
    int r = client.send_message(buf, 100); // 将消息发送给服务端
    cout << "********退出*******" << endl;
}