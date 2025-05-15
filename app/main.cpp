#include <iostream>
#include "scanner.hpp"
int main(){
    HeaderScanner scanner;

    auto tables = scanner.getTables();
    
    for(auto table : tables.value()){
        std::cout << table.toString();
    }


    return 0;
}