PROGRAMS = eigen-ex embed-red

CXX := clang++
CXXFLAGS = -g -std=c++11 -D EIGEN_MALLOC_ALREADY_ALIGNED=0

LIBS = boost_program_options
LDFLAGS =  -L /usr/lib $(LIBS:%=-l%)

FLAGS=-O2 $(CXXFLAGS) 

eigen-ex: eigen-ex.cpp
	$(CXX) $(FLAGS) -o $@ $<

embed-red: embed-red.cpp
	$(CXX) $(FLAGS) -o $@ $^

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)
