#include "heap.h"
#include "execution.h"
// #include "heap_json.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;


static pt::ptree ptreeOfObject(Object *o)
{
    pt::ptree p, pointsTo;
    
    p.put("id", o->getId());
    p.put("size", o->getSize());
    p.put("type", o->getType());
    p.put("threadId", o->getThread()->getId());
    p.put("createTime", o->getCreateTime());
    p.put("getDeathTime", o->getDeathTime());
    
    const EdgeMap &edges = o->getFields();
    
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        unsigned int ptrId = it->first;
        Object *tgt = it->second->getTarget();
        pt::ptree ptrData;

        ptrData.put("ptrId", ptrId);
        ptrData.put("tgtId", tgt->getId());
        
        pointsTo.push_back(std::make_pair("", ptrData));
    }

    p.add_child("pointsTo", pointsTo);
    return p;
}

pt::ptree heapPtree(HeapState *hs)
{
    pt::ptree p, objects;
    for (auto it = hs->begin(); it != hs->end(); ++it) {
        Object *o = it->second;
        objects.push_back(std::make_pair("", ptreeOfObject(o)));
    }
    p.add_child("objects", objects);
    return p;
}
