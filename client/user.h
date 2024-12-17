#ifndef __USER_H__
#define __USER_H__
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
enum TYPE
{
    QUIT = 0,
    T_LOGIN = 1,
    T_SELECT = 2,
    T_EDIT,
    S_LOGIN = 4,
    S_SELECT = 5
};
enum TYPE1
{
    YES,
    NO,
    LINE_FEED,
    END_OF_DATA

};
class user
{

public:
    user(string i, string n, int a, string p = "123456"); // 函数默认参数需要写在形参列表的靠右边

protected:
    string id;
    string name;   // 姓名
    int age;       // 年龄
    string passwd; // 密码
};
#endif