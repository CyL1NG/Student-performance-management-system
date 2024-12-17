#ifndef __STUDENT_H__
#define __STUDENT_H__
#include "user.h"
#include "mysqlite3.h"
#include "tcpserver.h"

class student : public user
{
public:
    student(string i, string n, string p = "123456", int a = 0, string classid = " ", string CID = " ", string MID = " ", string EID = " ", double c = 0, double m = 0, double e = 0);
    bool login(Mysqlite3 &sqlite3);
    bool select_INFO(int fd, TCPserver &server, Mysqlite3 &sqlite3);

private:
    // double score; // 学生成绩

    string class_ID;      // 班级号
    string Student_ID;    // 学号 --->主键
    string China_ID;      // 语文老师工号
    double China_score;   // 语文成绩
    string Math_ID;       // 数学老师工号
    double Math_score;    // 数学成绩
    string English_ID;    // 英语老师工号
    double English_score; // 英语成绩
};

#endif