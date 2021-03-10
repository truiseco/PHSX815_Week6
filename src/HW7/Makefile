# C++ compiler
CXX = g++

# necessary compiler flags for using ROOT (root.cern.ch) - remove these if you're not using root
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTGLIBS     = $(shell root-config --glibs)

# ROOT shared library flags
GLIBS += $(filter-out -stdlib=libc++ -pthread , $(ROOTGLIBS))

# some compiler flags
CXXFLAGS = -std=c++17 -g -Wall
# ROOT flags
CXXFLAGS += -fPIC $(filter-out -stdlib=libc++ -pthread , $(ROOTCFLAGS))

# location of source code
SRCDIR = ./src/

#location of header files
#INCLUDEDIR = ./include/

#CXXFLAGS += -I$(INCLUDEDIR)

# location of object files (from compiled library files)
OUTOBJ = ./obj/

#CC_FILES := $(wildcard src/*.cpp)
#HH_FILES := $(wildcard include/*.h)
#OBJ_FILES := $(addprefix $(OUTOBJ),$(notdir $(CC_FILES:.cc=.o)))

# targets to make
all: CoSim.x

CoSim.x: CoSim.o Random.o
	g++ $(CXXFLAGS) CoSim.o Random.o $(GLIBS) -o CoSim.x

CoSim.o: CoSim.cpp Random.h
	g++ $(CXXFLAGS) -c $(GLIBS) CoSim.cpp

Random.o: Random.h Random.cpp
	g++ -std=c++17 -g -Wall -c Random.cpp

#$(OUTOBJ)%.o: src/%.cpp include/%.h
#	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean-up target (make clean)
clean:
	rm -f *.x
	rm -rf *.dSYM
	rm -f *.o
