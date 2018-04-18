PROGRAMS = simulator_driver planar_discrete_driver eigen-ex component_test
	
CXX := clang++
CXXFLAGS = -g -std=c++11

LIBS = boost_program_options
LDFLAGS =  -L /usr/lib $(LIBS:%=-l%)

FLAGS=-O2 $(CXXFLAGS) 

eigen-ex: eigen-ex.cpp
	$(CXX) $(FLAGS) -o $@ $<
	
component_test: component_test.cpp embed.o
	g++ $(FLAGS) -o $@ $^

simulator_driver: simulator_driver.o simulator.o execution.o heap.o classinfo.o tokenizer.o components.o
	g++ $(FLAGS) -o $@ $^

planar_discrete_driver: planar_discrete_driver.cpp rollup_tree.o planar_tree_discrete.o
	$(CXX) $(FLAGS)$(LDFLAGS) -o $@ $^

embed.o: embed.cpp embed.h components.h
	$(CXX) $(FLAGS)  -c -o $@ $<

simulator.o: simulator.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS)  -c -o $@ $<

components.o: components.cpp heap.h components.h
	$(CXX) $(FLAGS)  -c -o $@ $<

planar_tree_discrete.o: planar_tree_discrete.cpp planar_tree_discrete.h
	$(CXX) $(FLAGS)  -c -o $@ $<

rollup_tree.o: rollup_tree.cpp rollup_tree.h
	$(CXX) $(FLAGS)  -c -o $@ $<

simulator_driver.o: simulator_driver.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS)  -c -o $@ $<

execution.o: execution.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

heap.o: heap.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

classinfo.o: classinfo.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

tokenizer.o: tokenizer.cpp classinfo.h tokenizer.h
	$(CXX) $(FLAGS) -c -o $@ $<

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)
