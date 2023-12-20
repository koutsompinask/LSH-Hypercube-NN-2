CC = g++
CFLAG = -g -O3 -fopenmp
SRC = lsh.cpp cube.cpp hashcube.cpp hashtable.cpp helper.cpp graph_search.cpp gnns.cpp mrng.cpp
OBJLSH = lsh.o hashtable.o helper.o
OBJCUBE = cube.o hypercube.o helper.o
OBJCLSTR = clustering.o helper.o hashtable.o hypercube.o clusteringMethods.o
OBJGRAPH = graph_search.o gnns.o mrng.o hashtable.o helper.o
OBJ = $(OBJCLSTR) $(OBJCUBE) $(OBJGRAPH) $(OBJLSH)
EX = graph_search lsh cube cluster

all : $(EX)

lsh : $(OBJLSH)
	$(CC) $^ -o  $@

cube : $(OBJCUBE)
	$(CC) $^ -o  $@

cluster : $(OBJCLSTR)
	$(CC) $^ -o  $@

graph_search: $(OBJGRAPH)
	$(CC) $^ -o  $@ -fopenmp

lsh.o: lsh.cpp helper.o hashtable.o
	$(CC) -c lsh.cpp $(CFLAG) -o lsh.o

cube.o: cube.cpp helper.o hypercube.o
	$(CC) -c cube.cpp $(CFLAG) -o cube.o

graph_search.o : graph_search.cpp gnns.o mrng.o helper.o hashtable.o
	$(CC) -c graph_search.cpp $(CFLAG) -o graph_search.o

mrng.o : mrng.h mrng.cpp hashtable.o helper.o graph.h 
	$(CC) -c mrng.cpp $(CFLAG) -o mrng.o

gnns.o : gnns.h gnns.cpp hashtable.o helper.o graph.h
	$(CC) -c gnns.cpp $(CFLAG) -o gnns.o

hashtable.o : hashtable.cpp hashtable.h
	$(CC) -c hashtable.cpp $(CFLAG) -o hashtable.o

hypercube.o : hypercube.cpp hypercube.h
	$(CC) -c hypercube.cpp $(CFLAG) -o hypercube.o

helper.o : helper.cpp helper.h
	$(CC) -c helper.cpp $(CFLAG) -o helper.o

clustering.o : clustering.cpp helper.o clusteringMethods.o 
	$(CC) -c clustering.cpp $(CFLAG) -o clustering.o

clusteringMethods.o : clusteringMethods.cpp hashtable.o hypercube.o helper.o
	$(CC) -c clusteringMethods.cpp $(CFLAG) -o clusteringMethods.o
	
clean:
	rm -rf $(OBJ) $(EX)