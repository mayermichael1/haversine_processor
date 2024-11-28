CXX=g++
OPT=-O0
DEPFLAGS=-MP -MD

INCLUDEDIRS=. include
BUILDDIR=./build

CUSTOMFLAGS=-D DEBUG
CXXFLAGS= $(OPT) $(DEPFLAGS) $(CUSTOMFLAGS) -g $(foreach dir, $(INCLUDEDIRS), -I$(dir))

# binary gen
SRCDIRS=gen shared

CPPFILES:=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

OBJFILES:=$(patsubst %.cpp, %.o, $(CPPFILES))
OBJFILES:=$(addprefix $(BUILDDIR)/, $(OBJFILES))
OBJFILES:=$(patsubst ./,,$(OBJFILES))

DEPFILES:=$(patsubst %.cpp, %.d, $(CPPFILES))
DEPFILES:=$(addprefix $(BUILDDIR)/, $(DEPFILES))
DEPFILES:=$(patsubst ./,,$(DEPFILES))

BINARY=$(BUILDDIR)/gen_json

# binary process
SRCDIRS2=process shared

CPPFILES2:=$(foreach dir, $(SRCDIRS2), $(wildcard $(dir)/*.cpp))

OBJFILES2:=$(patsubst %.cpp, %.o, $(CPPFILES2))
OBJFILES2:=$(addprefix $(BUILDDIR)/, $(OBJFILES2))
OBJFILES2:=$(patsubst ./,,$(OBJFILES2))

DEPFILES2:=$(patsubst %.cpp, %.d, $(CPPFILES2))
DEPFILES2:=$(addprefix $(BUILDDIR)/, $(DEPFILES2))
DEPFILES2:=$(patsubst ./,,$(DEPFILES2))

BINARY2=$(BUILDDIR)/process_json

# testing process
SRCDIRS3=testing shared

CPPFILES3:=$(foreach dir, $(SRCDIRS3), $(wildcard $(dir)/*.cpp))

OBJFILES3:=$(patsubst %.cpp, %.o, $(CPPFILES3))
OBJFILES3:=$(addprefix $(BUILDDIR)/, $(OBJFILES3))
OBJFILES3:=$(patsubst ./,,$(OBJFILES3))

DEPFILES3:=$(patsubst %.cpp, %.d, $(CPPFILES3))
DEPFILES3:=$(addprefix $(BUILDDIR)/, $(DEPFILES3))
DEPFILES3:=$(patsubst ./,,$(DEPFILES3))

BINARY3=$(BUILDDIR)/test

# targets

all: $(BINARY) $(BINARY2) $(BINARY3)

$(BINARY) : $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BINARY2) : $(OBJFILES2)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BINARY3) : $(OBJFILES3)
	nasm -f elf64 testing/bandwidth_test.asm -o build/bandwidth_test.o
	$(CXX) $(CXXFLAGS) build/bandwidth_test.o -o $@ $^

$(BUILDDIR)/%.o : %.cpp
	mkdir -p $(addprefix $(BUILDDIR)/, $(dir $<))
	$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(DEPFILES)

.PHONY : clean
clean: 
	rm -f $(BINARY)
	rm -f $(DEPFILES)
	rm -f $(OBJFILES)
	rm -f $(BINARY2)
	rm -f $(DEPFILES2)
	rm -f $(OBJFILES2)
	rm -f compile_commands.json
