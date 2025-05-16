#include <iostream>
#include "scanner.hpp"
#include "sql_builder.hpp"
int main(){
    HeaderScanner scanner;
    SQLBuilder builder;

    auto tables = scanner.getTables().value();
    
  

    builder.createTables(tables);

    return 0;
}