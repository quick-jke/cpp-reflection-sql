#ifndef TABLE
#define TABLE

#include <string>
#include <vector>
#include "field.hpp"

class Table{
public:
    Table(const std::string& name, std::vector<Field> fields, std::vector<std::string> dependencies);

private:
    std::string name_;
    std::vector<Field> fields_;
    std::vector<std::string> dependencies_;

};


#endif