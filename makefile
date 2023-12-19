CC = g++
CFLAG = -g -O3 -fopenmp
SRC = lsh.cpp cube.cpp hashcube.cpp hashtable.cpp helper.cpp graph_search.cpp gnns.cpp mrng.cpp
OBJLSH = lsh.o hashtable.o helper.o
OBJ = graph_search.o gnns.o mrng.o hashtable.o helper.o

EX = graph_search lsh

all : $(EX)

lsh : $(OBJLSH)
	$(CC) $^ -o  $@

graph_search: $(OBJ)
	$(CC) $^ -o  $@ -fopenmp

lsh.o: lsh.cpp helper.o hashtable.o
	$(CC) -c lsh.cpp $(CFLAG) -o lsh.o

graph_search.o : graph_search.cpp gnns.o mrng.o helper.o hashtable.o
	$(CC) -c graph_search.cpp $(CFLAG) -o graph_search.o

mrng.o : mrng.h mrng.cpp hashtable.o helper.o graph.h 
	$(CC) -c mrng.cpp $(CFLAG) -o mrng.o

gnns.o : gnns.h gnns.cpp hashtable.o helper.o graph.h
	$(CC) -c gnns.cpp $(CFLAG) -o gnns.o

hashtable.o : hashtable.cpp hashtable.h
	$(CC) -c hashtable.cpp $(CFLAG) -o hashtable.o

helper.o : helper.cpp helper.h
	$(CC) -c helper.cpp $(CFLAG) -o helper.o
	
clean:
	rm -rf $(OBJ) $(EX)