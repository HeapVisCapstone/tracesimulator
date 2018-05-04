#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <vector>
#include <fstream>
#include <deque>
#include <algorithm>
#include "classinfo.h"

class ClassNode {
public:
    ClassNode(std::string cls) : className(cls) {}
    std::string className;
    std::vector<ClassNode *> subclasses;
    void addClass(std::string parentName, std::string childName);
};

class ClassHierarchy {
private:
    ClassNode *root;

    static string unnormalize(std::string className) {
        std::replace(className.begin(), className.end(), '/', '.');
        className.erase(0, 1);
        return className;
    }

    static string normalize(std::string className) {
        std::replace(className.begin(), className.end(), '.', '/');
        className.insert(0, 1, 'L');
        return className;
    }
    
    ClassNode *getClassNode(std::string className)
    {
        std::deque<ClassNode *> searchQueue;

        ClassNode *thisNode = this->root;
        searchQueue.push_front(thisNode);
            
        while (!searchQueue.empty()) {
            for (auto it = thisNode->subclasses.begin();
                 it != thisNode->subclasses.end();
                 ++it) {
                searchQueue.push_front(*it);
            }
        
            thisNode = searchQueue.front();
            searchQueue.pop_front();
            
            if (thisNode->className == className) {
                return thisNode;
            }
        }

        return nullptr;
    }
    
public:

    // unnormalized = . separated, normalized = / separated
    bool normalized = true;
    
    ClassHierarchy(bool isNormalized) : normalized(isNormalized)
    {
        this->root = new ClassNode("java.lang.Object");
    }
    // invariant: able to add only if parent is in the tree
    void addClass(std::string parentName, std::string childName);
    std::vector<std::string> getSubclasses(std::string className);
    std::string getParent(std::string className);
    static ClassHierarchy fromDumpFile(std::ifstream inputFileStream,
        bool normalize);
};

#endif
