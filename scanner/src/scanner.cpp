#include "scanner.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// === Переводим строку в enum Macro ===
OPTION toMacro(const std::string& str) {
    if (str == "ID") return OPTION::ID;
    if (str == "ONE_TO_ONE") return OPTION::ONE_TO_ONE;
    if (str == "ONE_TO_MANY") return OPTION::ONE_TO_MANY;
    if (str == "MANY_TO_MANY") return OPTION::MANY_TO_MANY;
    if (str == "MANY_TO_ONE") return OPTION::MANY_TO_ONE;
    throw std::runtime_error("Unknown macro: " + str);
}

void HeaderScanner::scanAllEntities(const std::string& rootDir) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootDir)) {
            if (isEntityFile(entry.path())) {
                try {
                    StructInfo info = parseStructFromFile(entry.path().string());
                    printStructInfo(info);
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing file " << entry.path() << ": " << e.what() << "\n";
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
    }
}

bool HeaderScanner::isEntityFile(const fs::path& path) {
    return fs::is_regular_file(path) &&
           path.extension() == ".hpp" &&
           path.filename().string().find(".entity") != std::string::npos;
}

StructInfo HeaderScanner::parseStructFromFile(const std::string& filePath) {
    fs::path path = fs::absolute(filePath);

    if (!fs::exists(path)) {
        throw std::runtime_error("File does not exist: " + path.string());
    }

    if (!fs::is_regular_file(path)) {
        throw std::runtime_error("Path is not a regular file: " + path.string());
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    size_t structPos = content.find("struct ");
    if (structPos == std::string::npos)
        throw std::runtime_error("No 'struct' found in file");

    size_t nameStart = content.find_first_not_of(" \t\n\r", structPos + 7);
    size_t nameEnd = content.find_first_of(" \t\n\r{", nameStart);

    StructInfo result;
    result.name = content.substr(nameStart, nameEnd - nameStart);

    size_t bodyStart = content.find("{", nameEnd);
    size_t bodyEnd = content.find("};", bodyStart);

    std::string structBody = content.substr(bodyStart + 1, bodyEnd - bodyStart - 1);

    parseFields(structBody, result.fields);

    return result;
}

std::string normalizeType(const std::string& type) {
    std::string result = type;

    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    result.erase(std::remove(result.begin(), result.end(), '*'), result.end());

    size_t pos = 0;
    while ((pos = result.find("std::", pos)) != std::string::npos) {
        result.replace(pos, 5, "");
    }

    pos = 0;
    while ((pos = result.find("::", pos)) != std::string::npos) {
        result.replace(pos, 2, "");
    }

    size_t vectorPos = result.find("vector");
    if (vectorPos != std::string::npos && result.size() > vectorPos + 6) {
        size_t openBracket = result.find('<', vectorPos);
        size_t closeBracket = result.rfind('>');

        if (openBracket != std::string::npos && closeBracket != std::string::npos && openBracket < closeBracket) {
            std::string innerType = result.substr(openBracket + 1, closeBracket - openBracket - 1);

            innerType.erase(std::remove(innerType.begin(), innerType.end(), '*'), innerType.end());
            innerType.erase(std::remove(innerType.begin(), innerType.end(), ' '), innerType.end());

            return innerType;
        }
    }

    return result;
}

void HeaderScanner::parseFields(const std::string& body, std::vector<FieldInfo>& fields) {
    std::istringstream ss(body);
    std::string line;

    while (std::getline(ss, line, ';')) {
        trim(line);
        if (line.empty()) continue;

        FieldInfo field;

        std::vector<std::string> tokens;
        std::string token;
        size_t pos = 0;

        while (pos < line.size()) {
            char c = line[pos];

            if (std::isspace(c)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (c == '<' || c == '>' || c == ':' || c == '*' || c == '&') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                token += c;
                tokens.push_back(token);
                token.clear();
            } else {
                token += c;
            }

            ++pos;
        }

        if (!token.empty()) {
            tokens.push_back(token);
        }

        size_t macroCount = 0;
        while (macroCount < tokens.size()) {
            std::string token = tokens[macroCount];
            bool isKnown = false;

            for (const auto& m : {"ID", "ONE_TO_ONE", "ONE_TO_MANY", "MANY_TO_MANY", "MANY_TO_ONE"}) {
                if (token == m) {
                    field.attributes.push_back(toMacro(token));
                    isKnown = true;
                    break;
                }
            }

            if (!isKnown) break;
            macroCount++;
        }

        std::vector<std::string> typeTokens(tokens.begin() + macroCount, tokens.end());
        if (typeTokens.empty()) continue;

        std::string type;
        std::string name = typeTokens.back(); 
        for (size_t i = 0; i < typeTokens.size() - 1; ++i) {
            if (!type.empty()) type += " ";
            type += typeTokens[i];
        }

        field.name = name;
        field.type = normalizeType(type);  

        fields.push_back(field);
    }
}

void HeaderScanner::trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

std::string HeaderScanner::trimCopy(std::string s) {
    ltrim(s);
    return s;
}

void HeaderScanner::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void HeaderScanner::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void HeaderScanner::printStructInfo(const StructInfo& info) {
    // std::cout << "Structure name: " << info.name << "\n";
    // for (const auto& field : info.fields) {
    //     std::cout << "  Field: " << field.name
    //               << ", Type: " << field.type
    //               << ", Attributes: { ";
    //     for (const auto& attr : field.attributes) {
    //         switch (attr) {
    //             case OPTION::ID: std::cout << "ID "; break;
    //             case OPTION::ONE_TO_ONE: std::cout << "ONE_TO_ONE "; break;
    //             case OPTION::ONE_TO_MANY: std::cout << "ONE_TO_MANY "; break;
    //             case OPTION::MANY_TO_MANY: std::cout << "MANY_TO_MANY "; break;
    //             case OPTION::MANY_TO_ONE: std::cout << "MANY_TO_ONE "; break;
    //         }
    //     }
    //     std::cout << "}\n";
    // }

    std::cout << "\n-- SQL for table " << info.name << " --\n";
    std::cout << "CREATE TABLE IF NOT EXISTS " << info.name << " (\n";

    bool hasPrimaryKey = false;
    std::vector<std::string> columns;
    std::vector<std::string> foreignKeys;
    std::vector<std::string> manyToManyTables;

    for (const auto& field : info.fields) {
        bool isToMany = false;
        for (auto attr : field.attributes) {
            if (attr == OPTION::ONE_TO_MANY || attr == OPTION::MANY_TO_MANY) {
                isToMany = true;
                break;
            }
        }
        if (isToMany) continue;

        std::string columnName = field.name;
        std::string columnType;

        if (field.type == "int") {
            columnType = "INT";
        } else if (field.type == "string") {
            columnType = "VARCHAR(255)";
        } else {
            bool isManyToOne = false;
            bool isOneToOne = false;

            for (auto attr : field.attributes) {
                if (attr == OPTION::MANY_TO_ONE) isManyToOne = true;
                if (attr == OPTION::ONE_TO_ONE) isOneToOne = true;
            }

            if (isManyToOne || isOneToOne) {
                std::string fkColumn = columnName + "_id";
                columnType = "INT";
                std::string fkConstraint = "FOREIGN KEY (" + fkColumn + ") REFERENCES " + normalizeType(field.type) + "(id)";
                foreignKeys.push_back(fkConstraint);
                columnName = fkColumn;

                if (isOneToOne) {
                    columnName += " UNIQUE";
                }
            } else {
                columnType = "TEXT";
            }
        }

        bool isID = false;
        for (auto attr : field.attributes) {
            if (attr == OPTION::ID) {
                isID = true;
                break;
            }
        }

        if (isID && !hasPrimaryKey) {
            columnType += " PRIMARY KEY AUTO_INCREMENT";
            hasPrimaryKey = true;
        }

        columns.push_back("    " + columnName + " " + columnType);
    }

    if (!hasPrimaryKey) {
        columns.insert(columns.begin(), "    id INT PRIMARY KEY AUTO_INCREMENT");
    }

    for (size_t i = 0; i < columns.size(); ++i) {
        std::cout << columns[i];
        if (i < columns.size() - 1 || !foreignKeys.empty()) {
            std::cout << ",";
        }
        std::cout << "\n";
    }

    for (size_t i = 0; i < foreignKeys.size(); ++i) {
        std::cout << "    " << foreignKeys[i];
        if (i < foreignKeys.size() - 1) {
            std::cout << ",";
        }
        std::cout << "\n";
    }

    std::cout << ") ENGINE=InnoDB;\n\n";

    for (const auto& field : info.fields) {
        bool isManyToMany = false;
        for (auto attr : field.attributes) {
            if (attr == OPTION::MANY_TO_MANY) {
                isManyToMany = true;
                break;
            }
        }

        if (isManyToMany) {
            std::string otherTableName = normalizeType(field.type);

            std::string tableName = info.name + "_" + otherTableName;

            std::string sql =
                "CREATE TABLE IF NOT EXISTS " + tableName + " (\n" +
                "    " + info.name + "_id INT,\n" +
                "    " + otherTableName + "_id INT,\n" +
                "    PRIMARY KEY (" + info.name + "_id, " + otherTableName + "_id),\n" +
                "    FOREIGN KEY (" + info.name + "_id) REFERENCES " + info.name + "(id),\n" +
                "    FOREIGN KEY (" + otherTableName + "_id) REFERENCES " + otherTableName + "(id)\n" +
                ") ENGINE=InnoDB;\n";

            std::cout << "-- SQL for many-to-many link table --\n";
            std::cout << sql << "\n";
        }
    }
}