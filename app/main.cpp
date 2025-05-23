#include <iostream>
#include "scanner.hpp"
#include "sql_builder.hpp"
using namespace quick::ultra;
int main(){
    HeaderScanner scanner;
    // SQLBuilder builder;

    auto [tables, deps] = scanner.getTablesAndDependencies();

    for(auto table : tables){
        std::cout << table.toString();
    }

    for(auto dep : deps){
        std::cout << optionToString(dep.first) << std::endl << dep.second.first << ":" << dep.second.second << std::endl;
    }
    
  


    return 0;
}