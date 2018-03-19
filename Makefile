PROGRAMS = simulator jsontree-ex planar_discrete_driver

CXX := g++
CXXFLAGS = -g -std=c++11

LIBS =
LDFLAGS = $(LIBS:%=-l%)

FLAGS=-O2 $(CXXFLAGS)

simulator: simulator.o execution.o heap.o classinfo.o tokenizer.o analyze.o heap_json.o
	g++ $(FLAGS) -o $@ $^

jsontree-ex: jsontree-ex.cpp jsontree.h
	$(CXX) $(FLAGS) -o $@ $<

planar_discrete_driver: planar_discrete_driver.cpp rollup_tree.o planar_tree_discrete.o
	$(CXX) $(FLAGS) -o $@ $^

planar_tree_discrete.o: planar_tree_discrete.cpp planar_tree_discrete.h
	$(CXX) $(FLAGS)  -c -o $@ $<

rollup_tree.o: rollup_tree.cpp rollup_tree.h
	$(CXX) $(FLAGS)  -c -o $@ $<

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
