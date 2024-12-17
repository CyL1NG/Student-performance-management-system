#include "student.h"
#include "mysqlite3.h"
#include "tcpserver.h"
#include <string.h>

student::student(string i, string n, string p, int a, string classid, string CID, string MID, string EID, double c, double m, double e) : user{i, n, a, p}
{
    // score = s;

    class_ID = classid; // 班级号

    China_ID = CID;    // 语文老师工号
    China_score = c;   // 语文成绩
    Math_ID = MID;     // 数学老师工号
    Math_score = m;    // 数学成绩
    English_ID = EID;  // 英语老师工号
    English_score = e; // 英语成绩
}

bool student::login(Mysqlite3 &sqlite3)
{
    // 准备sql语句 ....
    char sql[200];
    sprintf(sql, "select 密码 from 学生信息表 where 姓名='%s' and 学号='%s';", name.c_str(), id.c_str());
    cout << sql << endl;
    vector<vector<string>> v = sqlite3.select(sql);
    if (v.empty())
    {
        cout << "用户名或者学号错误" << endl;
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
    学生查询
*/
bool student::select_INFO(int fd, TCPserver &server, Mysqlite3 &sqlite3)
{
    // 准备sql语句 ....
    char sql[200];
    sprintf(sql, "select * from 学生信息表 where 姓名='%s' and 学号='%s';", name.c_str(), id.c_str());
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
    const char *endMessage = "END_OF_DATA";                         // 定义结束标识符
    unsigned char endMessageBuffer[20];                             // 确保数组大小足够
    strcpy(reinterpret_cast<char *>(endMessageBuffer), endMessage); // 将字符串复制到 unsigned char 数组中

    r = server.send_message(fd, endMessageBuffer, strlen(endMessage) + 1);
    if (r == -1)
    {
        cout << "发送结束标识符失败" << endl;
        return false;
    }
    // cout << "发送结束标识符，返回值 r = " << r << endl;

    return true; // 成功发送所有信息
}