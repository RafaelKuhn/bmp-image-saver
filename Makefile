CMD = g++ src/main.cpp -o main.exe

all: compile run

compile:
	@echo -e "compiling\n"
	@$(CMD)

run:
	@echo -e "running\n"
	@./main.exe