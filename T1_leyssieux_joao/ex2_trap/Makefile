all: mpi_trap

mpi_trap: mpi_trap_input.c
	gcc -g -Wall -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" -o mpi_trap mpi_trap_input.c -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -lm

run_trap: mpi_trap
	mpiexec -n 4 mpi_trap.exe

clean:
	del mpi_trap.exe