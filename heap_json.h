#ifndef HEAP_JSON_H_
#define HEAP_JSON_H_

#include "heap.h"
#include "jsontree.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

boost::property_tree::ptree heapPtree(HeapState *hs);


#endif
