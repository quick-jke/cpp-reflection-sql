#ifndef FIELD
#define FIELD

#include "option.hpp"
#include "var.hpp"
#include <string>
#include <set>
#include <iostream>
#include "sql.hpp"

class Field{
public:
    Field(const std::string& name, const std::string& type, std::set<OPTION> options);

    std::string getSQLNormalize();

    // VAR getType();

    std::string toString();

private:
    std::string name_;
    std::string type_;
    // VAR type_;
    std::set<OPTION> options_;
};

#endif