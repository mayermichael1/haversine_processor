# build command for individual files
# build command to build the bin
# create dependencies using gcc
# find files automatically by a wildcard
# create object files and dep files by substituting c files
#
CXX=g++
OPT=-O0
DEPFLAGS=-MP -MD
CXXFLAGS= $(OPT) $(DEPFLAGS) -g 

BUILDDIR=./output
BINARY=$(BUILDDIR)/gen_json.out


SRCDIRS=. src

CPPFILES:=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
OBJFILES:=$(addprefix output/, $(addsuffix .o, $(basename $(notdir $(CPPFILES)))))
DEPFILES:=$(addprefix output/, $(addsuffix .d, $(basename $(notdir $(CPPFILES)))))

all: $(BINARY)

$(BINARY) : $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(DEPFILES)

.PHONY : clean
clean: 
	rm $(BINARY)
