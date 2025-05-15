#include "table.hpp"
Table::Table(){}

Table::Table(const std::string& name, std::vector<Field> fields, std::set<std::string> dependencies) 
    : name_(name), fields_(fields), dependencies_(dependencies){}

std::string Table::getName(){
    return name_;
}

std::vector<Field> Table::getFields(){
    return fields_;
}

std::set<std::string> Table::getDependencies(){
    return dependencies_;
}

std::string Table::toString(){
    std::string fields;
    std::string dependencies;
    for(auto field : fields_){
        fields += field.toString() + "\n";
    }

    for(int i = 0; i < fields_.size(); ++i){
        if(i + 1 != fields_.size()){
            fields += fields_[i].toString() + "\n";
        }else{
            fields += fields_[i].toString();
        }
    }

    for(auto dep : dependencies_){
        dependencies += " " + dep;
    }
    return "Table:\n\t" + name_ + "\nfields\n" + fields + "\ndeps:\n\t[" + dependencies + "]\n\n";
}