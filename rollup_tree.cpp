#include "rollup_tree.h"

#include <set>
#include <string>

int                   avoidCount = 0;
std::set<std::string> found = std::set<std::string>();
std::string           defaultName = "autogen";



std::string NameValidator::fresh() {

    return NameValidator::fresh(defaultName);
}

std::string NameValidator::fresh(std::string name) {
    while(not found.insert(name).second) {
        name = name + std::to_string(avoidCount++); 
    }

    return name;
}