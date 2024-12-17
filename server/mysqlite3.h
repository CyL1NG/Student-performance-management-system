#ifndef __MYSQLITE3_H__
#define __MYSQLITE3_H__

#include <iostream>
#include <vector>
#include "sqlite3.h"

using std::string;
using std::to_string;
using std::vector;

class Mysqlite3
{
public:
    Mysqlite3(const char *pathname);
    ~Mysqlite3();

    vector<vector<string>> select(const char *sql);
    bool not_select(const char *sql);

private:
    sqlite3 *pdb;
};

#endif
