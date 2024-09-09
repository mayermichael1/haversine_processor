# build command for individual files
# build command to build the bin
# create dependencies using gcc
# find files automatically by a wildcard
# create object files and dep files by substituting c files
#
CXX=g++
OPT=-O0
CXXFLAGS= $(OPT) -g

BUILDDIR=./output
BINARY=$(BUILDDIR)/gen_json.out

all: $(BINARY)

$(BINARY) : gen_json.cpp
	$(CXX) $(CXXFLAGS) -o $@ gen_json.cpp

.PHONY : clean
clean: 
	rm $(BUILDDIR)/*
