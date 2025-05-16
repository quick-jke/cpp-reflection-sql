#ifndef SQL_BUILDER_HPP
#define SQL_BUILDER_HPP

#include <iostream>
#include "table.hpp"
#include "sql.hpp"
#include <set>

#include <map>
#include <queue>
#include <sstream>

class SQLBuilder{
public:
    SQLBuilder();


    static void createTables(std::set<Table> tables);


};

#endif