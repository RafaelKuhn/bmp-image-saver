# configure output
EXECUTABLE = main.exe
OBJDIR = obj

# configure input
SRCDIR = src

# configure files to be compiled into .r.o (release) or .d.o (debug) objects
FILES = main.cpp types.cpp bmp.cpp


# which compiler and flags
CC = g++
CPP_FLAGS = -Wall
DEBUG_FLAGS = -Wall -Wextra -pedantic -D DEBUG_MODE

OBJS = $(FILES:%.cpp=%.o)
BIN_OBJS = $(addprefix $(OBJDIR)/, $(OBJS))

RELEASE_OBJS = $(BIN_OBJS:%.o=%.r.o)
DEBUG_OBJS = $(BIN_OBJS:%.o=%.d.o)

# create ./obj directory if doesnt exist
$(shell [ -d $(OBJDIR) ] || mkdir $(OBJDIR) )


all: main

# main target
main: $(RELEASE_OBJS)
	@echo "[make] linking main objects"
	$(CC) $(CPP_FLAGS) -o $(EXECUTABLE) $(RELEASE_OBJS)

# build objects for main target (first delete debug objects)
$(OBJDIR)/%.r.o: src/%.cpp
	@rm -f $(DEBUG_OBJS)
	$(CC) $(CPP_FLAGS) -c $< -o $@


# debug target
debug: $(DEBUG_OBJS)
	@echo "[make] linking debug objects"
	$(CC) $(DEBUG_FLAGS) -o $(EXECUTABLE) $(DEBUG_OBJS)

# build objects for debug target (first delete release objects)
$(OBJDIR)/%.d.o: src/%.cpp
	@rm -f $(RELEASE_OBJS)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@


run:
	@echo "[make] running"
	@./$(EXECUTABLE)

clean:
	@rm -rf $(OBJDIR)
	@rm -f *.exe