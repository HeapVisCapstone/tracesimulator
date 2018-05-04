#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <vector>
#include <fstream>
#include <deque>
#include <map>
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

    
public:

    // unnormalized = . separated, normalized = / separated
    bool normalized = true;
    std::map<std::string, ClassNode<D>*> nodemap; 

    ClassHierarchy(bool isNormalized) : normalized(isNormalized), nodemap()
    {
        string rootname = "java.lang.Object";
        root = new ClassNode<D>(rootname);
        nodemap[rootname] = root;
    }

    ClassNode<D> *getClassNode(std::string className)
    {

        if (nodemap.count(className)) {
            return nodemap[className];
        }
        return nullptr;
    }

    // invariant: able to add only if parent is in the tree
    void addClass(string parentName, string childName)
    {
        if (normalized) {
            parentName = normalize(parentName);
            childName  = normalize(childName);
        }
        
        auto parentNode = getClassNode(parentName);

        if (!parentNode) {
            throw runtime_error("Invalid insertion into hierarchy");
        }

        if (nodemap.count(childName)) {
            std::cerr << "Encountered duplicate name: " << childName
                      << "Discarding" << endl;
            return;
        }

 
        auto *newNode = new ClassNode<D>(childName);

        parentNode->subclasses.push_back(newNode);
        nodemap[childName] = newNode;

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
