#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;
using namespace std;

#include "tokenizer.h"
#include "classinfo.h"
#include "execution.h"
#include "heap.h"
#include "heap_json.h"

class Object;
class CCNode;

// ----------------------------------------------------------------------
//   Globals

// -- The heap
HeapState Heap;

// -- Execution state
ExecState Exec(1);

// -- Turn on debugging
bool debug = false;

// ----------------------------------------------------------------------
//   Analysis

void sanity_check()
{
          // -- Sanity check
          /*
          const ObjectMap& fields = obj->getFields();
          for (ObjectMap::const_iterator p = fields.begin();
               p != fields.end();
               p++)
            {
              Object * target = (*p).second;
              if (target) {
                if (Now > target->getDeathTime()) {
                  // -- Not good: live object points to a dead object
                  printf(" Live object %s points-to dead object %s\n",
                         obj->info().c_str(), target->info().c_str());
                }
              }                  
            }
          }
          */

        /*
        if (Now > obj->getDeathTime() && obj->getRefCount() != 0) {
          nonzero_ref++;
          printf(" Non-zero-ref-dead %X of type %s\n", obj->getId(), obj->getType().c_str());
        }
        */
}

bool member(Object * obj, const ObjectSet & theset)
{
    return theset.find(obj) != theset.end();
}

int compute_roots(ObjectSet & roots)
{
    unsigned int live = 0;
    for (ObjectMap::iterator p = Heap.begin();
	 p != Heap.end();
	 p++)
	{
	    Object * obj = (*p).second;
	    if (obj) {
		if (obj->isLive(Exec.Now())) {
		    live++;
		    if (obj->getRefCount() == 0)
			roots.insert(obj);
		}
	    }
	}

    return live;
}

unsigned int closure(ObjectSet & roots, ObjectSet & premarked, ObjectSet & result)
{
    unsigned int mark_work = 0;

    vector<Object *> worklist;

    // -- Initialize the worklist with only unmarked roots
    for (ObjectSet::iterator i = roots.begin();
	 i != roots.end();
	 i++)
	{
	    Object * root = *i;
	    if ( ! member(root, premarked))
		worklist.push_back(root);
	}

    // -- Do DFS until the worklist is empty
    while (worklist.size() > 0) {
	Object * obj = worklist.back();
	worklist.pop_back();
	result.insert(obj);
	mark_work++;

	const EdgeMap& fields = obj->getFields();
	for (EdgeMap::const_iterator p = fields.begin();
	     p != fields.end();
	     p++)
	    {
		Edge * edge = (*p).second;
		Object * target = edge->getTarget();
		if (target) {
		    // if (target->isLive(Exec.Now())) {
		    if (! member(target, premarked) &&
			! member(target, result))
			{
			    worklist.push_back(target);
			}
		    // } else {
		    // cout << "WEIRD: Found a dead object " << target->info() << " from " << obj->info() << endl;
		    // }
		}
	    }
    }

    return mark_work;
}

unsigned int count_live(ObjectSet & objects, unsigned int at_time)
{
    int count = 0;
    // -- How many are actually live
    for (ObjectSet::iterator p = objects.begin();
	 p != objects.end();
	 p++)
	{
	    Object * obj = (*p);
	    if (obj->isLive(at_time))
		count++;
	}

    return count;
}

// ----------------------------------------------------------------------
//   Read and process trace events

void *read_trace_file(FILE * f)
{
    Tokenizer t(f);
  
    unsigned int method_id;
    unsigned int object_id;
    unsigned int target_id;
    unsigned int field_id;
    unsigned int thread_id;
    unsigned int exception_id;
    Object * obj;
    Object * target;
    Method * method;

    // -- Allocation time
    unsigned int AllocationTime = 0;

    while ( ! t.isDone()) {
	t.getLine();
	if (t.isDone()) break;

	if (Exec.Now() % 1000000 == 1)
	    cout << "  Method time: " << Exec.Now() << "   Alloc time: " << AllocationTime << endl;

	switch (t.getChar(0)) {
	case 'A':
	case 'I':
	case 'N':
	case 'P':
	case 'V':
	    {
		// -- Allocate: a new object is created
		//
		//    o = new T();
		//
		//    The <id> is a unique ID for this object and is
		//    used to refer to it in all future events.  The
		//    <size> is size in bytes; <type> is the type;
		//    <site> is a unique identifier for this
		//    allocation site; <els> is the number of
		//    elements, in the case of array allocations.
		//
		// A/I/N/P/V <id> <size> <type> <site> [<els>] <threadid>
		//     0       1    2      3      4      5         5/6
		// -- Extract the fields from the log entry
		unsigned int thrdid = (t.numTokens() == 6) ? t.getInt(5) : t.getInt(6);
		Thread * thread = Exec.getThread(thrdid);
		unsigned int els  = (t.numTokens() == 6) ? 0 : t.getInt(5);
		AllocSite * as = ClassInfo::TheAllocSites[t.getInt(4)];

		// -- Create a new object in the heap model
		obj = Heap.allocate(t.getInt(1), t.getInt(2), t.getChar(0), t.getString(3), as, els, thread, Exec.Now());

		// -- Allocation "time" is just running count of total bytes allocated
		unsigned int old_alloc_time = AllocationTime;
		AllocationTime += obj->getSize();
	    }
	    break;

	case 'U':
	    // -- Update
	    //
	    //    This entry records a pointer store:
	    //
	    //      obj.field = new_target;
	    //
	    //    Although not strictly necessary, we also record the
	    //    old target of the pointer (that is, what was in the
	    //    field obj.field before the assignment.
	    
	    // U <old-target> <object> <new-target> <field> <thread>
	    // 0      1          2         3           4        5
	    // -- Extract the fields
	    obj = Heap.get(t.getInt(2));
	    target = Heap.get(t.getInt(3));
	    if (obj && target) {
		unsigned int field_id = t.getInt(4);
		// -- Add an edge in our heap representation
		Edge * new_edge = Heap.make_edge(obj, field_id, target, Exec.Now());

		obj->updateField(new_edge, Exec.Now()); // Not sure if this is needed
	    }
        
	    break;

	case 'D':
	    // -- Object death
	    //
	    //    This record indicates that an object became unreachable at 
	    //    this time. This is the earliest possible time that any
	    //    garbage collector could reclaim it.
	    // D <object>
	    // 0    1
	    obj = Heap.get(t.getInt(1));
	    if (obj)
		obj->makeDead(Exec.Now());
	    break;

	case 'M':
	    // -- Enter a method call
	    //
	    //    This kind of entry (along with the 'E' entry) play the roll
	    //    of time markers, rather than heap events. We tick our logical
	    //    clock each time we enter or exit a method call. We also record
	    //    the receiver object (if there is one).
	    // M <methodid> <receiver> <threadid>
	    // 0      1         2           3
	    method_id = t.getInt(1);
	    method = ClassInfo::TheMethods[method_id];
	    thread_id = t.getInt(3);
	    // -- This call updates our representation of call stack (e.g., by pushing
	    //    the current method. It can also build a full call tree.
	    Exec.Call(method, thread_id);
	    break;

	case 'E':
	case 'X':
	    // -- Exit a method call
	    //
	    //    Records when the program returns from a method. This event also
	    //    ticks the logical clock.
	    // E <methodid> <receiver> [<exceptionobj>] <threadid>
	    // 0      1         2             3             3/4
	    method_id = t.getInt(1);
	    method = ClassInfo::TheMethods[method_id];
	    thread_id = (t.numTokens() == 4) ? t.getInt(3) : t.getInt(4);
	    // -- This call effectively pops the stack. It also makes sure that the
	    //    current top of the stack representation is the same as the
	    //    method we're returning from, as a sanity check.
	    Exec.Return(method, thread_id);
	    break;

	case 'T':
	    // -- Not used
	    // T <methodid> <receiver> <exceptionobj>
	    // 0      1          2           3
	    break;
      
	case 'H':
	    // -- Not used
	    // H <methodid> <receiver> <exceptionobj>
	    break;

	case 'R':
	    // -- Not uses
	    break;
      
	default:
	    // cout << "ERROR: Unknown entry " << t.curChar() << endl;
	    break;
	}
    }
}

// ----------------------------------------------------------------------

void analyze();

int main(int argc, char * argv[])
{
    cout << "Read names file..." << endl;
    ClassInfo::read_names_file(argv[1]);
    cout << "Start trace..." << endl;
    FILE * f = fdopen(0, "r");
    read_trace_file(f);
    cout << "Done at time " << Exec.Now() << endl;
    Heap.end_of_program(Exec.Now());
    pt::ptree heapPtree = Heap.toPtree();

    std::fstream jsonS;
    jsonS.open("object.json", std::fstream::out | std::fstream::trunc);
    pt::write_json(jsonS, heapPtree);
    analyze();

    jsonS.close();
}

