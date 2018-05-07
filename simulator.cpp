#include "simulator.h"


pair<HeapState, ExecState> read_trace_file(const char* nameFile, FILE *f) {
  ExecState Exec(1);

  ClassInfo::read_names_file(nameFile);


  Tokenizer t(f);

  HeapState Heap;

  unsigned int method_id;
  unsigned int object_id;
  unsigned int target_id;
  unsigned int field_id;
  unsigned int thread_id;
  unsigned int exception_id;
  Object *obj;
  Object *target;
  Method *method;

  // -- Allocation time
  unsigned int AllocationTime = 0;

  while (!t.isDone()) {
    t.getLine();
    if (t.isDone()) {
        break;
    }

    if (Exec.Now() % 1000000 == 1)
      cout << "  Method time: " << Exec.Now()
           << "   Alloc time: " << AllocationTime << endl;

    switch (t.getChar(0)) {
      case 'A':
      case 'I':
      case 'N':
      case 'P':
      case 'V': {
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
        Thread *thread = Exec.getThread(thrdid);
        unsigned int els = (t.numTokens() == 6) ? 0 : t.getInt(5);
        AllocSite *as = ClassInfo::TheAllocSites[t.getInt(4)];

        // -- Create a new object in the heap model
        obj = Heap.allocate(t.getInt(1), t.getInt(2), t.getChar(0),
                            t.getString(3), as, els, thread, Exec.Now());

        // -- Allocation "time" is just running count of total bytes allocated
        unsigned int old_alloc_time = AllocationTime;
        AllocationTime += obj->getSize();
      } break;

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
          Edge *new_edge = Heap.make_edge(obj, field_id, target, Exec.Now());

          obj->updateField(new_edge, Exec.Now());  // Not sure if this is needed
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
        if (obj) obj->makeDead(Exec.Now());
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
        // -- This call updates our representation of call stack (e.g., by
        // pushing
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
  return make_pair(Heap, Exec);
}