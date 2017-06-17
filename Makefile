PROJECT := scanner

OBJDIR := obj
SRCDIR := src
LIBDIR := lib

LIBRARY := $(LIBDIR)/lib$(PROJECT).so

CXXSTD := -std=c++14
WARNINGS := -Wall -Werror -Wextra
INCLUDES := -Iinclude
CFLAGS += -$(CXXSTD) $(WARNINGS) -O3 

CXX += $(CFLAGS)

SOURCES := $(shell find $(SRCDIR) -name '**.cc')
OBJECTS := $(shell echo $(SOURCES:.cc=.o) | sed 's/$(SRCDIR)/$(OBJDIR)/g')

all: $(LIBRARY)

$(LIBRARY): $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	@mkdir -p $(@D)
	$(CXX) -shared $^ $(LIBS) -o $@

$(OBJECTS): $(OBJDIR)%.o: $(SRCDIR)%.cc
	@mkdir -p $(@D)
	$(CXX) $< -c -fPIC -o $@
	
clean:
	rm -rfv $(OBJDIR) $(LIBDIR)
