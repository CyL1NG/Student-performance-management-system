#include "mysqlite3.h"
using std::cerr;
using std::cout;
using std::endl;
Mysqlite3::Mysqlite3(const char *pathname)
{
    // 打开数据库
    int r = sqlite3_open(pathname, &pdb);
    if (r != SQLITE_OK)
    {
        // 抛出异常
        // 需要更改
        printf("打开失败,错误码=%d\n", r);
        return;
    }
}

Mysqlite3::~Mysqlite3()
{
    sqlite3_close(pdb);
}

vector<vector<string>> Mysqlite3::select(const char *sql)
{
    vector<vector<string>> v; // 保存查询结果
    // 编译sql语句
    sqlite3_stmt *pStmt;
    int r = sqlite3_prepare_v2(pdb, sql, -1, &pStmt, NULL);
    if (r != SQLITE_OK)
    {
        cerr << "编译SQL语句失败, 错误码=" << r << ", 错误信息: " << sqlite3_errmsg(pdb) << endl;

        return v; // 返回空容器
    }

    int flag = 0;
    while (1)
    {
        // 执行sql语句
        r = sqlite3_step(pStmt);
        if (r == SQLITE_DONE)
        {
            break;
        }
        else if (r == SQLITE_ROW)
        {
            // 查询到了一条记录
            vector<string> row;
            // 获取字段数量
            int n = sqlite3_column_count(pStmt);
            int i;

            for (i = 0; i < n; i++)
            {
                switch (sqlite3_column_type(pStmt, i))
                {
                case SQLITE_INTEGER:
                    // printf("%d  ", sqlite3_column_int(pStmt, i));
                    row.push_back(to_string(sqlite3_column_int(pStmt, i)));
                    break;
                case SQLITE_FLOAT:
                    // printf("%lf  ", sqlite3_column_double(pStmt, i));
                    row.push_back(to_string(sqlite3_column_double(pStmt, i)));
                    break;
                case SQLITE_TEXT:
                    // printf("%s  ", sqlite3_column_text(pStmt, i));
                    row.push_back(reinterpret_cast<const char *>(sqlite3_column_text(pStmt, i)));
                    break;
                default:
                    row.push_back(""); // 如果类型未知，插入空字符串
                    break;
                }
            }
            v.push_back(row); // 将当前行添加到结果集中
        }
        else
        {
            printf("执行sql语句失败\n");
            sqlite3_finalize(pStmt); // 销毁sql语句
            return v;                // 返回空容器
        }
    }
    // 销毁sql语句
    sqlite3_finalize(pStmt);
    return v;
}

bool Mysqlite3::not_select(const char *sql)
{
    // 编译sql语句
    sqlite3_stmt *pStmt;
    int r = sqlite3_prepare_v2(pdb, sql, -1, &pStmt, NULL);
    if (r != SQLITE_OK)
    {
        cerr << "编译SQL语句失败, 错误码=" << r << ", 错误信息: " << sqlite3_errmsg(pdb) << endl;
        cout << "编译失败" << endl;
        return false;
    }

    // 执行sql语句

    r = sqlite3_step(pStmt);
    if (r != SQLITE_DONE)
    {
        // 销毁sql语句
        sqlite3_finalize(pStmt);
        cout << "执行失败" << endl;
        return false;
    }

    // 销毁sql语句
    sqlite3_finalize(pStmt);
    return true;
}

// 默认参数写在声明