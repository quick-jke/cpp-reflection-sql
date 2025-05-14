#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

enum OPTION {

    ONE_TO_ONE,
    ONE_TO_MANY,
    MANY_TO_MANY,
    MANY_TO_ONE,
    ID
};


struct FieldInfo {
    std::string type;
    std::string name;
    std::vector<OPTION> attributes;
};

struct StructInfo {
    std::string name;
    std::vector<FieldInfo> fields;
};

class HeaderScanner {
public:
    void scanAllEntities(const std::string& rootDir);

    StructInfo parseStructFromFile(const std::string& filePath);

private:
    bool isEntityFile(const std::filesystem::path& path);

    void parseFields(const std::string& body, std::vector<FieldInfo>& fields);

    void trim(std::string& s);
    std::string trimCopy(std::string s);
    void ltrim(std::string& s);
    void rtrim(std::string& s);
    std::string replaceAll(std::string str, const std::string& from, const std::string& to);

    void printStructInfo(const StructInfo& info);

    static const std::vector<std::string> KNOWN_MACROS;
};

#endif 