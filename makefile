ECHO := @

DEV_PLATFORM = DEV_PLATFORM_UNIX

DEV_PLATFORM_FLAGS := -D$(DEV_PLATFORM)
CC := $(ECHO)g++
CFLINKS := -pthread -lpthread
CEFLAGS_DEBUG := $(DEV_PLATFORM_FLAGS) -pthread -lm -std=c++11 -O2 -Wall -W -Wextra -Wdouble-promotion -pedantic -Wmissing-include-dirs -Wunused -Wuninitialized -Wextra -Wstrict-overflow=3 -Wtrampolines -Wfloat-equal -Wconversion -Wmissing-field-initializers -Wno-multichar -Wpacked -Winline -Wshadow
CEFLAGS_RELEASE := $(DEV_PLATFORM_FLAGS) -pthread -lm -std=c++11 -O3 -w
CEAFTER_FLAGS := -lws2_32
CEFLAGS := $(CEFLAGS_RELEASE)
EXAMPLES_FILES := $(wildcard ./examples/src/*.cpp)
EXAMPLES_FILES_LIST := $(addprefix ,$(notdir $(EXAMPLES_FILES:.cpp=)))
EXAMPLES_OBJ_FILES := $(addprefix ./examples/bin/,$(notdir $(EXAMPLES_FILES:.cpp=.o)))
EXAMPLES_EXE_FILES := $(addprefix ./examples/bin/,$(notdir $(EXAMPLES_FILES:.cpp=.exe)))
EXAMPLES_BUILT_FILES := $(addprefix ./examples/bin/,$(notdir $(wildcard ./examples/bin/*)))
EXAMPLES_NUMBER := $(words $(EXAMPLES_FILES))
EXAMPLES_MADE_NUMBER := %

CC_FLAGS=-I./include $(CEFLAGS)
LF_FLAGS=$(CFLINKS)
AF_FLAGS=$(CEAFTER_FLAGS)

ifneq (,$(filter clean clean-all,$(MAKECMDGOALS)))
ifeq (,$(EXAMPLES_BUILT_FILES))
$(error Directory is clean)
endif
endif


help:
	$(info The-Socket C/C++ Socket Handling Library)
	$(info Makefile)
	$(info To compile all examples use 'make all' or 'make examples' command)
	$(info To get list of all available examples use 'make get-examples' command)
	$(info To clean project use 'make clean' command)

rebuild: clean-all examples

clean: clean-all
	$(info Project cleaned. Done)

clean-all: clean-examples

all: examples

examples: examples_ $(EXAMPLES_OBJ_FILES) $(EXAMPLES_EXE_FILES) clean-examples-build
	$(info Examples compiled. Done)

examples_:
	$(info $(EXAMPLES_NUMBER) examples are available)

get-examples:
	$(info All available examples: $(EXAMPLES_FILES_LIST))

clean-examples:
	$(ECHO)rm $(EXAMPLES_BUILT_FILES)

clean-examples-build:
	$(info Cleaning build)
	$(ECHO)rm $(EXAMPLES_OBJ_FILES)

./examples/bin/%.exe: ./examples/bin/%.o
	$(CC) $(LF_FLAGS) -o $@ $< $(AF_FLAGS)

./examples/bin/%.o: ./examples/src/%.cpp
	$(info Compiling example: $<)
	$(CC) $(CC_FLAGS) -c -o $@ $< $(AF_FLAGS)
