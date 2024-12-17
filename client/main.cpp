#include <iostream>
#include "TCPclient.h"
#include "teacher.h"
#include "student.h"

void show_login_menu();
int Student_login(TCPclient &client);
int Teacher_login(TCPclient &client);

int main()
{
    // 创建一个客户端对象，连接好服务端
    TCPclient client{};
    client.connect_server("172.6.1.121", 8889);

    while (1)
    {
        show_login_menu(); // 显示登录菜单

        int x;
        cin >> x; // 获取用户输入

        switch (x)
        {
        case 1: // 学生登录
            Student_login(client);
            break;
        case 2: // 老师登录
            Teacher_login(client);
            break;
        case 0: // 退出程序
            cout << "退出程序" << endl;
            return 0;
        default:
            cout << "输入有误，请重新输入" << endl; // 无效输入反馈
        }
    }
}

/*
    显示登录菜单
*/
void show_login_menu()
{
    cout << endl;
    cout << "==============================" << endl;
    cout << "         登录菜单               " << endl;
    cout << "==============================" << endl;
    cout << "       输入 1 进行学生登录        " << endl;
    cout << "       输入 2 进行老师登录        " << endl;
    cout << "       输入 0 退出程序            " << endl;
    cout << "==============================" << endl;
    cout << "请输入你的选择: "; // 提示用户输入选择
}

/*
    处理学生登录
*/
int Student_login(TCPclient &client)
{
    string id, name, passwd;
    cout << "请依次输入你的学号、姓名、密码......(以空格隔开)" << endl;

    while (cin >> id >> name >> passwd)
    {
        student s{id, name, passwd}; // 创建一个学生对象
        if (s.show_login() == S_LOGIN)
        {
            if (!s.login_S(client))
            {
                cout << "登录失败,用户名或密码错误,请重新输入..." << endl;
                continue; // 继续循环输入
            }
            cout << "====登录成功====" << endl;

            while (true)
            {
                int action = s.show_INFO(); // 输入你要进行的操作
                if (action == S_SELECT)
                {
                    s.select_INFO(client); // 查询自己的信息
                }
                else if (action == QUIT)
                {
                    cout << "退出" << endl;
                    s.quit_S(client);
                    return 0; // 返回主菜单
                }
            }
        }
    }
    return 0; // 如果输入失败，返回主菜单
}

/*
    处理老师登录
*/
int Teacher_login(TCPclient &client)
{
    string id, name, passwd;
    cout << "请依次输入你的工号、姓名、密码......(以空格隔开)" << endl;

    while (1)
    {
        cin >> id >> name >> passwd;
        teacher t{id, name, passwd}; // 创建一个老师对象
        if (t.show_login() == T_LOGIN)
        {
            if (!t.login(client))
            {
                cout << "登录失败,用户名或密码错误,请重新输入..." << endl;
                return 0; // 继续循环输入
            }
            cout << "====登录成功====" << endl;

            while (1)
            {
                int action = t.show(); // 输入你要进行的操作
                if (action == T_SELECT)
                {
                    t.select_INFO(client); // 查询所有学生信息
                }
                else if (action == T_EDIT)
                {
                    // 修改指定学生信息
                    if (id[0] == 'C')
                        t.edit_INFO_C(client);
                    else if (id[0] == 'M')
                        t.edit_INFO_M(client);
                    else if (id[0] == 'E')
                        t.edit_INFO_E(client);
                }
                else if (action == QUIT)
                {
                    cout << "退出" << endl;
                    t.quit(client);
                    return 0; // 返回主菜单
                }
            }
        }
    }
    return 0; // 如果输入失败，返回主菜单
}