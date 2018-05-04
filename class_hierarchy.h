#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <vector>
#include <fstream>
#include <deque>
#include <algorithm>
#include "classinfo.h"

template <class D>
class ClassNode {
public:
    ClassNode(std::string cls) : className(cls) {}
    std::string className;
    std::vector<ClassNode *> subclasses;

    D data;

    // Unimplemented
    // void addClass(std::string parentName, std::string childName);

};

template <class D>
class ClassHierarchy {
private:
    ClassNode<D> *root;

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
    
    ClassNode<D> *getClassNode(std::string className)
    {
        std::deque<ClassNode<D> *> searchQueue;

        ClassNode<D> *thisNode = this->root;
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
        this->root = new ClassNode<D>("java.lang.Object");
    }

    // invariant: able to add only if parent is in the tree
    void addClass(string parentName, string childName)
    {
        ClassNode<D> *parentNode;
        if (this->normalized) {
            parentNode = this->getClassNode(normalize(parentName));
        } else {
            parentNode = this->getClassNode(parentName);
        }

        if (!parentNode) {
            throw runtime_error("Invalid insertion into hierarchy");
        }


        ClassNode<D> *newNode;
        if (this->normalized) {
            newNode = new ClassNode<D>(normalize(childName));
        } else {
            newNode = new ClassNode<D>(childName);
        }
        parentNode->subclasses.push_back(newNode);
    }

    std::vector<std::string> getSubclasses(std::string className)
    {
        ClassNode<D> *parentNode;
        if (this->normalized) {
            parentNode = this->getClassNode(normalize(className));
        } else {
            parentNode = this->getClassNode(className);
        }
        
        if (!parentNode) {
            throw runtime_error("Invalid insertion into hierarchy");
        }

        auto children = parentNode->subclasses;

        vector<string> childNames;
        
        for (auto it = children.begin(); it != children.end(); ++it) {
            string name = (*it)->className;

            if (this->normalized) {
                childNames.push_back(normalize(name));
            } else {
                childNames.push_back(name);
            }
        }

        return childNames;
    }

    // unimplemented
    // std::string getParent(std::string className);


    static ClassHierarchy fromDumpFile(std::ifstream inputFileStream, bool normalize)
    {
        ClassHierarchy ch(normalize);
        string child, parent;
        while (!inputFileStream.eof()) {
            string discard;
            inputFileStream >> child >> discard >> parent;
            ch.addClass(parent, child);
        }
        return ch;
    }
};

#endif
