#include "table.hpp"

Table::Table(const std::string& name, std::vector<Field> fields, std::vector<std::string> dependencies) 
    : name_(name), fields_(fields), dependencies_(dependencies){}