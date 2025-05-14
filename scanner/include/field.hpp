#ifndef FIELD
#define FIELD

#include "option.hpp"
#include "var.hpp"
#include <string>
#include <vector>

class Field{
public:
    Field(const std::string& name, VAR type, std::vector<OPTION> options);

private:
    std::string name_;
    VAR type_;
    std::vector<OPTION> options_;
};

#endif