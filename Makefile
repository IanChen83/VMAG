
CXX		:= g++
PROG	:= main

CPPFLAG	+= -O2 -std=c++11 -g

SRC		:= main.cpp

all: $(PROG)
default: $(PROG)

.PHONY	: all clean

OBJ		:= $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

$(PROG): $(OBJ)
	g++ $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(PROG) $(OBJ)
