CXX=g++
OPT=-O0
DEPFLAGS=-MP -MD

BUILDDIR=./build
SRCDIRS=. shared
INCLUDEDIRS=. include

CPPFILES:=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

OBJFILES:=$(patsubst %.cpp, %.o, $(CPPFILES))
OBJFILES:=$(addprefix $(BUILDDIR)/, $(OBJFILES))
OBJFILES:=$(patsubst ./,,$(OBJFILES))

DEPFILES:=$(patsubst %.cpp, %.d, $(CPPFILES))
DEPFILES:=$(addprefix $(BUILDDIR)/, $(DEPFILES))
DEPFILES:=$(patsubst ./,,$(DEPFILES))

BINARY=$(BUILDDIR)/gen_json.out

CXXFLAGS= $(OPT) $(DEPFLAGS) -g $(foreach dir, $(INCLUDEDIRS), -I$(dir))

# targets

all: $(BINARY)

$(BINARY) : $(OBJFILES)
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
	rm -f compile_commands.json
