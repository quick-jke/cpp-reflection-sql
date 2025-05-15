#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <set>
#include <optional>

#include "table.hpp"

struct TableComparator{
    bool operator()(Table t1, Table t2) const
    {
        return t1.getDependencies().size() < t2.getDependencies().size();
    }
};

class HeaderScanner {
public:
    std::optional<std::vector<Table>> getTables();
private:
    bool isEntityFile(const std::filesystem::path& path);
    Table getTableFromFile(const std::string& filePath);
    std::pair<std::vector<Field>, std::set<std::string>> getFieldsByBody(const std::string& body); 
};

#endif 