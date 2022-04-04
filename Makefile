
all: compile run

compile:
	@echo "[make] compiling"
	@g++ src/main.cpp -o main.exe

run:
	@echo "[make] running"
	@./main.exe