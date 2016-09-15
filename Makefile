CXX		:= g++
PROG	:= main

CPPFLAG	+= -std=c++11 -g

SRC		:= main.cpp vmag.cpp level.cpp range.cpp util.cpp vmagc.cpp

all: $(PROG)

.PHONY	: all clean

OBJ		:= $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

$(PROG): $(OBJ)
		$(CXX) $^ -o $@

%.o: %.cpp
		$(CXX) -c $(CPPFLAG) $< -o $@

clean:
		rm -rf $(PROG) $(OBJ)
