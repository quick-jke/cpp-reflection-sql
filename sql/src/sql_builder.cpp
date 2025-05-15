#include "sql_builder.hpp"

SQLBuilder::SQLBuilder(){

}


void SQLBuilder::createTable(Table table){
    std::cout << "\n-- SQL for table " << table.getName() << " --\n";
    std::cout << "CREATE TABLE IF NOT EXISTS " << table.getName() << " (\n";

    for(auto field : table.getFields()){
        std::cout << field.getSQLNormalize() << std::endl;
    }

    std::cout << ") ENGINE=InnoDB;\n\n";

}