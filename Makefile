CXX=g++
OPT=-O0
DEPFLAGS=-MP -MD

INCLUDEDIRS=. include
BUILDDIR=./build

CXXFLAGS= $(OPT) $(DEPFLAGS) -g $(foreach dir, $(INCLUDEDIRS), -I$(dir))

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

# targets

all: $(BINARY) $(BINARY2)

$(BINARY) : $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BINARY2) : $(OBJFILES2)
	$(CXX) $(CXXFLAGS) -o $@ $^

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
