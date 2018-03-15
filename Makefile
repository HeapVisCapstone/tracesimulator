
# SOURCES = $(wildcard *.cpp)
# HEADERS = $(wildcard *.h)

# OBJECTS = $(SOURCES:%.cpp=%.o)
PROGRAMS = simulator jsontree-ex

CXX := g++
CXXFLAGS = -g -std=c++11

LIBS =
LDFLAGS = $(LIBS:%=-l%)

# $(PROGRAM) : $(OBJECTS)
# 	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# %.o : %.cpp $(HEADERS)
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# .PHONY : clean
# clean :
# 	rm -f $(PROGRAM) $(OBJECTS)

FLAGS=-O2 $(CXXFLAGS)

simulator: simulator.o execution.o heap.o classinfo.o tokenizer.o analyze.o heap_json.o
	g++ $(FLAGS) -o $@ $^

jsontree-ex: jsontree-ex.cpp jsontree.h
	$(CXX) $(FLAGS) -o $@ $<

planar_discrete_dump: planar_discrete_dump.cpp rollup_tree.h
	$(CXX) $(FLAGS) -o $@ $<

simulator.o: simulator.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS)  -c -o $@ $<

analyze.o: analyze.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS)  -c -o $@ $<

execution.o: execution.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

heap.o: heap.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

classinfo.o: classinfo.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

tokenizer.o: tokenizer.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

heap_json.o: heap_json.cpp heap_json.h
	$(CXX) $(FLAGS) -c -o $@ $<

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)
