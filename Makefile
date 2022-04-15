# configure output
EXECUTABLE = test.exe
OBJDIR = obj
LIBDIR = lib
LIB_NAME = bmplib.lib
LIB_HEADER = bmplib.h

# configure input
SRCDIR = src
# configure files to be compiled into .r.o (release) or .d.o (debug) objects
# "main" shouldn't be compiled for library
LIB_FILES = bmp-types.cpp bmp.cpp
FILES = test.cpp $(LIB_FILES)

# confiure compiler and flags
CC = g++
CPP_FLAGS = -Wall
DEBUG_FLAGS = -Wall -Wextra -pedantic -D DEBUG_MODE

LIB_HEADERS = $(addprefix $(SRCDIR)/, $(LIB_FILES:%.cpp=%.h))
LIB_OBJS = $(addprefix $(OBJDIR)/, $(LIB_FILES:%.cpp=%.r.o))
RELEASE_OBJS = $(addprefix $(OBJDIR)/, $(FILES:%.cpp=%.r.o))
DEBUG_OBJS = $(addprefix $(OBJDIR)/, $(FILES:%.cpp=%.d.o))

# create obj directory if doesnt exist
$(shell [ -d $(OBJDIR) ] || mkdir $(OBJDIR) )


all: test lib

# test target
test: $(RELEASE_OBJS)
	@echo "[make] linking main objects"
	$(CC) $(CPP_FLAGS) -o $(EXECUTABLE) $(RELEASE_OBJS)

# debug target
debug: $(DEBUG_OBJS)
	@echo "[make] linking debug objects"
	$(CC) $(DEBUG_FLAGS) -o $(EXECUTABLE) $(DEBUG_OBJS)

# lib target
lib: $(LIB_OBJS)
# this creates lib folder and runs bash script
# to create library header (combines library headers in one)
	@[ -d $(OBJDIR) ] || mkdir $(LIBDIR)
	@./scripts/build-lib-header.sh $(LIBDIR) $(LIB_HEADERS)
	@echo "[make] archiving library"
	ar rcs -o $(LIBDIR)/$(LIB_NAME) $(LIB_OBJS)


# build objects for release targets (first deletes debug objects)
$(OBJDIR)/%.r.o: src/%.cpp
	@rm -f $(DEBUG_OBJS)
	$(CC) $(CPP_FLAGS) -c $< -o $@

# build objects for debug target (first deletes release objects)
$(OBJDIR)/%.d.o: src/%.cpp
	@rm -f $(RELEASE_OBJS)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@


run:
	@[ -f $(EXECUTABLE) ] && { \
		./$(EXECUTABLE); \
	} || { \
		echo "[make] executable not found, run 'make test' or 'make debug' first"; \
	}

clean:
	rm -rf $(OBJDIR)/*.o
	rm -f *.exe
	rm -f *.lib
	rm -f *.bmp