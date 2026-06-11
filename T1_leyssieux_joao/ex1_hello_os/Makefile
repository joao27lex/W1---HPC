all: hello_os hello_ring

hello_os: hello_os.c
	gcc -g -Wall -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" -o hello_os hello_os.c -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi

run_os: hello_os
	mpiexec -n 8 hello_os.exe

hello_ring: hello_ring.c
	gcc -g -Wall -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" -o hello_ring hello_ring.c -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi

clean:
	del hello_os.exe hello_ring.exe