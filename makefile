CC = g++
CFLAG = -g -O3 -fopenmp
SRC = hashtable.cpp helper.cpp graph_search.cpp gnns.cpp mrng.cpp
OBJ = graph_search.o gnns.o mrng.o hashtable.o helper.o
EX = graph_search

all : $(EX)

graph_search: $(OBJ)
	$(CC) $^ -o  $@ -fopenmp

graph_search.o : graph_search.cpp gnns.o mrng.o
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