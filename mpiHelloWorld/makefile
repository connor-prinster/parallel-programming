EXECS=mpi_hello_world
MPICC?=mpicc

all: ${EXECS}

mpi_hello_world: main.cpp
	${MPICC} -o main.cpp

clean:
	rm -f ${EXECS}
