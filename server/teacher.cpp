#include "teacher.h"
#include "mysqlite3.h"
#include "tcpserver.h"
#include <string.h>
using std::cerr;
using std::cout;
using std::endl;
teacher::teacher(string i, string n, string p, string j, int a) : user{i, n, a, p}
{
    JobTitle = j;
}

bool teacher::login(Mysqlite3 &sqlite3)
{
    // 准备sql语句 ....
    char sql[200];
    sprintf(sql, "select 密码 from 老师信息表 where 姓名='%s' and 工号='%s';", name.c_str(), id.c_str());
    cout << sql << endl;
    vector<vector<string>> v = sqlite3.select(sql);
    if (v.empty())
    {
        cout << "用户名或者工号错误" << endl;
        return false;
    }
    else
    {
        if (v[0][0] == passwd)
        {
            // cout << "登录成功" << endl;
            return true;
        }
        else
        {
            // cout << "密码错误" << endl;
            return false;
        }
    }
}

/*
    老师查询
*/
bool teacher::select_INFO(int fd, TCPserver &server, Mysqlite3 &sqlite3)
{
    // 准备sql语句 ....
    char sql[200];
    sprintf(sql, "SELECT * FROM 学生信息表;");
    // cout << sql << endl;
    int r;

    // 查询所有学生信息
    vector<vector<string>> v = sqlite3.select(sql);
    if (v.empty())
    {
        cout << "没有找到任何学生信息。" << endl;
        return false; // 如果没有结果，返回 false
    }

    // 将查询到的学生信息发送给客户端
    for (const auto &row : v)
    {
        // 假设 row 是一个包含每一行数据的字符串向量
        string message;
        for (const auto &field : row)
        {
            message += field + " "; // 将字段连接成一行
        }
        cout << "发送学生信息: " << message << endl;
        // cout << message.length() << endl;

        // 发送信息到客户端
        r = server.send_message(fd, (unsigned char *)message.c_str(), message.length());
        if (r == -1)
        {
            cout << "传送失败" << endl;
            return false;
        }
        // cout << "发送学生信息数据r=" << r << endl;
    }
    // 发送结束标识符
    const char *endMessage = "END_OF_DATA";         // 定义结束标识符
    unsigned char endMessageBuffer[20];             // 确保数组大小足够
    strcpy((char *)(endMessageBuffer), endMessage); // 将字符串复制到 unsigned char 数组中

    r = server.send_message(fd, endMessageBuffer, strlen(endMessage) + 1);
    if (r == -1)
    {
        cout << "发送结束标识符失败" << endl;
        return false;
    }
    // cout << "发送结束标识符，返回值 r = " << r << endl;

    return true; // 成功发送所有信息
}

void teacher::edit_INFO_C(int fd, TCPserver &server, Mysqlite3 &sqlite3)
{
    char sql[200];
    char buf[1000];
    char buf1[1000];
    double score = 0.0;
    // 接受客户端发送的信息
    // cout << "接受客户端发送的信息:......" << endl;
    int r = server.recv_message(fd, (unsigned char *)buf, 1000);
    buf[r] = '\0';
    sprintf(sql, "select * from 学生信息表 where 学号 = '%s';", buf);
    vector<vector<string>> v = sqlite3.select(sql); // 执行sql语句
    if (v.empty())
    {
        cout << "没有找到任何学生信息。" << endl;
        buf1[0] = NO;
        // r = server.send_message(fd, (unsigned char *)message.c_str(), message.length());
        return; // 如果没有结果，返回 false
    }

    // 将查询到的学生信息发送给客户端
    for (const auto &row : v)
    {
        // 假设 row 是一个包含每一行数据的字符串向量
        string message;
        for (const auto &field : row)
        {
            message += field + " "; // 将字段连接成一行
        }
        cout << "发送学生信息: " << message << endl;

        // 发送信息到客户端
        r = server.send_message(fd, (unsigned char *)message.c_str(), message.length());
        if (r == -1)
        {
            cout << "传送失败" << endl;
            return;
        }
        // cout << "发送学生信息数据r=" << r << endl;
    }
    // 发送结束标识符
    const char *endMessage = "END_OF_DATA";                         // 定义结束标识符
    unsigned char endMessageBuffer[20];                             // 确保数组大小足够
    strcpy(reinterpret_cast<char *>(endMessageBuffer), endMessage); // 将字符串复制到 unsigned char 数组中

    r = server.send_message(fd, endMessageBuffer, strlen(endMessage) + 1);
    if (r == -1)
    {
        cout << "发送结束标识符失败" << endl;
        return;
    }
    // cout << "发送结束标识符，返回值 r = " << r << endl;

    r = server.recv_message(fd, (unsigned char *)buf1, 1000);
    buf1[r] = '\0';
    // update 学生信息表 set 年龄=30 where 姓名='关羽';//把关羽的年龄修改为30
    puts((char *)buf1);
    sprintf(sql, "update 学生信息表 set 语文成绩=%s where 学号='%s';", buf1, buf); // 修改成绩
    cout << "修改学生成绩的sql语句:" << sql << endl;

    // 执行sql语句
    if (sqlite3.not_select(sql))
    {
        cout << "执行成功" << endl;
    }
    else
    {
        cout << "执行失败" << endl;
    }
    sprintf(sql, "SELECT * FROM 学生信息表; ");
    return;
}

void teacher::edit_INFO_M(int fd, TCPserver &server, Mysqlite3 &sqlite3)
{
    char sql[200];
    char buf[1000];
    char buf1[1000];
    double score = 0.0;
    // 接受客户端发送的信息
    // cout << "接受客户端发送的信息:......" << endl;
    int r = server.recv_message(fd, (unsigned char *)buf, 1000);
    buf[r] = '\0';
    sprintf(sql, "select * from 学生信息表 where 学号 = '%s';", buf);
    vector<vector<string>> v = sqlite3.select(sql); // 执行sql语句
    if (v.empty())
    {
        cout << "没有找到任何学生信息。" << endl;

        return; // 如果没有结果，返回 false
    }

    // 将查询到的学生信息发送给客户端
    for (const auto &row : v)
    {
        // 假设 row 是一个包含每一行数据的字符串向量
        string message;
        for (const auto &field : row)
        {
            message += field + " "; // 将字段连接成一行
        }
        cout << "发送学生信息: " << message << endl;

        // 发送信息到客户端
        r = server.send_message(fd, (unsigned char *)message.c_str(), message.length());
        if (r == -1)
        {
            cout << "传送失败" << endl;
            return;
        }
        // cout << "发送学生信息数据r=" << r << endl;
    }
    // 发送结束标识符
    const char *endMessage = "END_OF_DATA";                         // 定义结束标识符
    unsigned char endMessageBuffer[20];                             // 确保数组大小足够
    strcpy(reinterpret_cast<char *>(endMessageBuffer), endMessage); // 将字符串复制到 unsigned char 数组中

    r = server.send_message(fd, endMessageBuffer, strlen(endMessage) + 1);
    if (r == -1)
    {
        cout << "发送结束标识符失败" << endl;
        return;
    }
    // cout << "发送结束标识符，返回值 r = " << r << endl;

    r = server.recv_message(fd, (unsigned char *)buf1, 1000);
    buf1[r] = '\0';
    // update 学生信息表 set 年龄=30 where 姓名='关羽';//把关羽的年龄修改为30
    puts((char *)buf1);
    sprintf(sql, "update 学生信息表 set 数学成绩=%s where 学号='%s';", buf1, buf); // 修改成绩
    cout << "修改学生成绩的sql语句:" << sql << endl;

    // 执行sql语句
    if (sqlite3.not_select(sql))
    {
        cout << "执行成功" << endl;
    }
    else
    {
        cout << "执行失败" << endl;
    }
    // sprintf(sql, "SELECT * FROM 学生信息表; ");
    return;
}

void teacher::edit_INFO_E(int fd, TCPserver &server, Mysqlite3 &sqlite3)
{
    char sql[200];
    char buf[1000];
    char buf1[1000];
    double score = 0.0;
    // 接受客户端发送的信息
    // cout << "接受客户端发送的信息:......" << endl;
    int r = server.recv_message(fd, (unsigned char *)buf, 1000);
    buf[r] = '\0';
    sprintf(sql, "select * from 学生信息表 where 学号 = '%s';", buf);
    vector<vector<string>> v = sqlite3.select(sql); // 执行sql语句
    if (v.empty())
    {
        cout << "没有找到任何学生信息。" << endl;
        return; // 如果没有结果，返回 false
    }

    // 将查询到的学生信息发送给客户端
    for (const auto &row : v)
    {
        // 假设 row 是一个包含每一行数据的字符串向量
        string message;
        for (const auto &field : row)
        {
            message += field + " "; // 将字段连接成一行
        }
        cout << "发送学生信息: " << message << endl;

        // 发送信息到客户端
        r = server.send_message(fd, (unsigned char *)message.c_str(), message.length());
        if (r == -1)
        {
            cout << "传送失败" << endl;
            return;
        }
        // cout << "发送学生信息数据r=" << r << endl;
    }
    // 发送结束标识符
    const char *endMessage = "END_OF_DATA";                         // 定义结束标识符
    unsigned char endMessageBuffer[20];                             // 确保数组大小足够
    strcpy(reinterpret_cast<char *>(endMessageBuffer), endMessage); // 将字符串复制到 unsigned char 数组中

    r = server.send_message(fd, endMessageBuffer, strlen(endMessage) + 1);
    if (r == -1)
    {
        cout << "发送结束标识符失败" << endl;
        return;
    }
    // cout << "发送结束标识符，返回值 r = " << r << endl;

    r = server.recv_message(fd, (unsigned char *)buf1, 1000);
    buf1[r] = '\0';
    // update 学生信息表 set 年龄=30 where 姓名='关羽';//把关羽的年龄修改为30
    puts((char *)buf1);
    sprintf(sql, "update 学生信息表 set 英语成绩=%s where 学号='%s';", buf1, buf); // 修改成绩
    cout << "修改学生成绩的sql语句:" << sql << endl;

    // 执行sql语句
    if (sqlite3.not_select(sql))
    {
        cout << "执行成功" << endl;
    }
    else
    {
        cout << "执行失败" << endl;
    }
    sprintf(sql, "SELECT * FROM 学生信息表; ");
    return;
}