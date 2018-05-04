#include "class_hierarchy.h"
#include <deque>

using namespace std;

void ClassHierarchy::addClass(string parentName, string childName)
{
    ClassNode *parentNode;
    if (this->normalized) {
        parentNode = this->getClassNode(normalize(parentName));
    } else {
        parentNode = this->getClassNode(parentName);
    }

    if (!parentNode) {
        throw runtime_error("Invalid insertion into hierarchy");
    }


    ClassNode *newNode;
    if (this->normalized) {
        newNode = new ClassNode(normalize(childName));
    } else {
        newNode = new ClassNode(childName);
    }
    parentNode->subclasses.push_back(newNode);
}

vector<string> ClassHierarchy::getSubclasses(string className)
{
    ClassNode *parentNode;
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

ClassHierarchy ClassHierarchy::fromDumpFile(ifstream inputFileStream, bool normalize)
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
