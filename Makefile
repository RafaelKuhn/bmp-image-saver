# configure output
EXECUTABLE = test.exe
OBJ_DIR = obj
LIB_DIR = lib
LIB_NAME = bmplib.lib
DEBUG_LIB_NAME = bmplib_debug.lib

# configure input
SRCDIR = src
# configure files to be compiled into .r.o (release) or .d.o (debug) objects
# "test.cpp" shouldn't be compiled for library
LIB_FILES = types.cpp bmp.cpp
FILES = test.cpp $(LIB_FILES)

# confiure compiler and flags
CC = g++
CPP_FLAGS = -Wall
DEBUG_FLAGS = -Wall -Wextra -pedantic -D DEBUG_MODE

LIB_HEADERS = $(addprefix $(SRCDIR)/, $(LIB_FILES:%.cpp=%.h))
LIB_OBJS = $(addprefix $(OBJ_DIR)/, $(LIB_FILES:%.cpp=%.r.o))
LIB_DEBUG_OBJS = $(addprefix $(OBJ_DIR)/, $(LIB_FILES:%.cpp=%.d.o))

RELEASE_OBJS = $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.r.o))
DEBUG_OBJS = $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.d.o))


# create obj directory if doesnt exist
$(shell [ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR) )


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
	@[ -d $(LIB_DIR) ] || mkdir $(LIB_DIR)
	@./scripts/build-lib-header.sh $(LIB_DIR) $(LIB_HEADERS)
	@echo "[make] archiving library"
	ar rcs -o $(LIB_DIR)/$(LIB_NAME) $(LIB_OBJS)

lib-debug: $(LIB_DEBUG_OBJS)
# this creates lib folder and copy-paste headers as-is
	@[ -d $(LIB_DIR) ] || mkdir $(LIB_DIR)
	@cp $(LIB_HEADERS) $(LIB_DIR)
# @echo "[make] archiving library"
	ar rcs -o $(LIB_DIR)/$(DEBUG_LIB_NAME) $(LIB_DEBUG_OBJS)


# build objects for release targets (first deletes debug objects)
$(OBJ_DIR)/%.r.o: src/%.cpp
	@rm -f $(DEBUG_OBJS)
	$(CC) $(CPP_FLAGS) -c $< -o $@

# build objects for debug target (first deletes release objects)
$(OBJ_DIR)/%.d.o: src/%.cpp
	@rm -f $(RELEASE_OBJS)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@


run:
	@[ -f $(EXECUTABLE) ] && { \
		./$(EXECUTABLE); \
	} || { \
		echo "[make] executable not found, run 'make test' or 'make debug' first"; \
	}

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -f *.exe
	rm -f *.lib
	rm -f *.bmp