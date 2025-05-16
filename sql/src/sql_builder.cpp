#include "sql_builder.hpp"

SQLBuilder::SQLBuilder(){

}


std::string getReferencedTableName(const std::string& type) {
    std::string result = type;

    size_t template_open = result.find('<');
    if (template_open != std::string::npos) {
        size_t template_close = result.rfind('>');
        if (template_close != std::string::npos && template_close > template_open) {
            result = result.substr(template_open + 1, template_close - template_open - 1);
        }
    }

    while (!result.empty() && (result.back() == '*' || result.back() == ' ')) {
        result.pop_back();
    }

    return result;
}

void generateManyToManyTables(std::set<Table> tables) {
    std::set<std::pair<std::string, std::string>> processed;

    for (auto table : tables) {
        for (auto field : table.getFields()) {
            if (field.getOptions().count(OPTION::MANY_TO_MANY)) {
                std::string other_table = getReferencedTableName(field.getType());

                std::string table1 = table.getName();
                std::string table2 = other_table;

                if (table1 > table2) std::swap(table1, table2);

                if (processed.count({table1, table2})) {
                    continue;
                }

                processed.insert({table1, table2});

                std::string join_table = table1 + "_" + table2;

                std::cout << "CREATE TABLE IF NOT EXISTS " << join_table << " (" << std::endl;
                std::cout << "\t" << table1 << "_id INT REFERENCES " << table1 << "(id)," << std::endl;
                std::cout << "\t" << table2 << "_id INT REFERENCES " << table2 << "(id)," << std::endl;
                std::cout << "\tPRIMARY KEY (" << table1 << "_id, " << table2 << "_id)" << std::endl;
                std::cout << ");\n" << std::endl;
            }
        }
    }
}

bool analysField(Field field, std::string table_name) {
    std::string option_str;
    std::string foreign_key;

    std::string sql_type;
    std::string referenced_table = getReferencedTableName(field.getType());

    if (field.getOptions().count(OPTION::MANY_TO_MANY)) {
        return false; 
    }

    if (field.getType() == "int") {
        sql_type = " INT";
    } else if (field.getType() == "std::string") {
        sql_type = " VARCHAR(255)";
    } else {
        sql_type = "_id INT";
        foreign_key = " REFERENCES " + referenced_table + "(id)";
    }

    if (field.getOptions().count(OPTION::ID)) {
        option_str = " PRIMARY KEY AUTO_INCREMENT";
    } else if (field.getOptions().count(OPTION::ONE_TO_ONE)) {
        option_str += " UNIQUE";
        foreign_key = " REFERENCES " + referenced_table + "(id)";
    } else if (field.getOptions().count(OPTION::MANY_TO_ONE)) {
        foreign_key = " REFERENCES " + referenced_table + "(id)";
    }

    std::cout << "\t" << field.getName() << sql_type << option_str << foreign_key;
    return true;
}

void SQLBuilder::createTables(std::set<Table> tables) {
    for (auto table : tables) {
        std::cout << "CREATE TABLE IF NOT EXISTS " << table.getName() << " (" << std::endl;

        bool first = true;
        for (auto field : table.getFields()) {
            if (field.getOptions().count(OPTION::MANY_TO_MANY) || field.getOptions().count(OPTION::ONE_TO_MANY)) {
                continue; 
            }

            if (!first) {
                std::cout << "," << std::endl;
            }

            analysField(field, table.getName());
            first = false;
        }

        std::cout << "\n);\n" << std::endl;
    }
    generateManyToManyTables(tables);
}
