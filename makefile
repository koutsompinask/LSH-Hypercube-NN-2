CC = g++
CFLAG = -g -O2
SRC = lsh.cpp hashtable.cpp helper.cpp hypercube.cpp graph_search.cpp gnns.cpp
OBJLSH = lsh.o hashtable.o helper.o
OBJCUBE = cube.o hypercube.o helper.o 
OBJGRAPH = graph_search.o gnns.o hashtable.o helper.o
OBJ = lsh.o cube.o hypercube.o hashtable.o helper.o graph_search.o gnns.o
EX = lsh cube graph_search

all : $(EX)

graph_search: $(OBJGRAPH)
	$(CC) $^ -o  $@

lsh : $(OBJLSH)
	$(CC) $^ -o  $@

cube : $(OBJCUBE)
	$(CC) $^ -o  $@

graph_search.o : graph_search.cpp gnns.o
	$(CC) -c graph_search.cpp $(CFLAG) -o graph_search.o

gnns.o : gnns.h gnns.cpp hashtable.o helper.o graph.h
	$(CC) -c gnns.cpp $(CFLAG) -o gnns.o

lsh.o : lsh.cpp hashtable.o helper.o
	$(CC) -c lsh.cpp $(CFLAG) -o lsh.o

cube.o : cube.cpp hypercube.o helper.o
	$(CC) -c cube.cpp $(CFLAG) -o cube.o

hashtable.o : hashtable.cpp hashtable.h
	$(CC) -c hashtable.cpp $(CFLAG) -o hashtable.o

hypercube.o : hypercube.cpp hypercube.h
	$(CC) -c hypercube.cpp $(CFLAG) -o hypercube.o

helper.o : helper.cpp helper.h
	$(CC) -c helper.cpp $(CFLAG) -o helper.o
	
clean:
	rm -rf $(OBJ) $(EX)