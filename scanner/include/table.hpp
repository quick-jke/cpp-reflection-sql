#ifndef TABLE
#define TABLE

#include <string>
#include <vector>
#include <set>
#include "field.hpp"

class Table{
public:
    Table(const std::string& name, std::vector<Field> fields, std::set<std::string> dependencies);
    Table();
    std::string getName();
    std::vector<Field> getFields();
    std::set<std::string> getDependencies();

    std::string toString();

private:
    std::string name_;
    std::vector<Field> fields_;
    std::set<std::string> dependencies_;

};


#endif