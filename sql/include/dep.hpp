#ifndef SQL_DEPENDENCY_HPP
#define SQL_DEPENDENCY_HPP


#include <iostream>
#include <map>
#include <set>
#include <string>
#include "option.hpp"



using tableNames = std::pair<std::string, std::string>;
using dependency = std::pair<tableNames, OPTION>;


struct depComparator {
    bool operator()(const dependency& lhs, const dependency& rhs) const {
        auto orderedLhs = orderPair(lhs.first);
        auto orderedRhs = orderPair(rhs.first);

        return orderedLhs < orderedRhs;
    }

private:
    tableNames orderPair(const tableNames& pair) const {
        if (pair.first <= pair.second)
            return pair;
        else
            return {pair.second, pair.first};
    }
};

using dependencies = std::set<dependency, depComparator>;

class DependencyManager{
public:
    DependencyManager(){};
    void add(dependency dep){
        deps_.insert(dep);
    }
    std::string toString(){
        std::string str;
        for(auto dep : deps_){
            std::string kind;
            str += "{" + dep.first.first + ":" + dep.first.second + "}, ";
            switch (dep.second)
            {
            case MANY_TO_MANY:{
                kind = "MANY_TO_MANY";
                break;
            }
            case MANY_TO_ONE:{
                kind = "MANY_TO_ONE";
                break;
            }
            case ONE_TO_ONE:{
                kind = "ONE_TO_ONE";
                break;
            }
            case ONE_TO_MANY:{
                kind = "ONE_TO_MANY";
                break;
            }
            default:
                break;
            }
        
            str += kind;
        }
        return str;
    }
private:
    dependencies deps_;
};


#endif