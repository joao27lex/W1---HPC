# Compiling and running the hello_gather
CC = mpicc
CFLAGS = -O2 -Wall
LDFLAGS = -lm

# Name of the program and src file
PROG = minmax
SRC = $(PROG).c

# random default p
P = 4

# Makefile rules
all: $(PROG)

# Compiles the program
$(PROG): $(SRC)
	$(CC) $(CFLAGS) -o $(PROG) $(SRC) $(LDFLAGS)

# Runs with mpiexec and the specified number of processes
run: $(PROG)
	mpiexec -n $(P) ./$(PROG)

# Cleans the compiled program
clean:
	rm -f $(PROG)