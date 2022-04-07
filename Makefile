DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CPPFLAGS = -Wall -Wextra -pedantic -D DEBUG_MODE
else
    CPPFLAGS = -Wall
endif

# CPPFLAGS = -Wall -Wextra -pedantic -D DEBUG_MODE
# CPPFLAGS = -Wall

all: main

main: main.o types.o bmp.o
	@echo "[make] compiling and linking main.exe"
	g++ $(CPPFLAGS) -o main.exe bmp.o main.o types.o

main.o: src/main.cpp
	g++ $(CPPFLAGS) -c src/main.cpp -o main.o

types.o: src/types.cpp
	g++ $(CPPFLAGS) -c src/types.cpp -o types.o

bmp.o: src/bmp.cpp
	g++ $(CPPFLAGS) -c src/bmp.cpp -o bmp.o


run:
	@echo "[make] running"
	@./main.exe

clean:
	@rm -f *.o
	@rm -f *.exe