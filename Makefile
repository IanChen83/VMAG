
CXX		:= g++
PROG	:= main

CPPFLAG	+= -std=c++11 -g

SRC		:= main.cpp vmag.cpp vmag_ds.cpp

all: $(PROG)
default: $(PROG)

.PHONY	: all clean

OBJ		:= $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

$(PROG): $(OBJ)
	g++ $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CPPFLAG) $< -o $@

clean:
	rm -rf $(PROG) $(OBJ)
