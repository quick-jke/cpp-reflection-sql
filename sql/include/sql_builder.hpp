#ifndef SQL_BUILDER_HPP
#define SQL_BUILDER_HPP

#include <iostream>
#include "table.hpp"
#include "sql.hpp"

class SQLBuilder{
public:
    SQLBuilder();

    static void createTable(Table table);


};

#endif