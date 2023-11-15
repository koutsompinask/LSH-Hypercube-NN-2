#ifndef gm
#define gm
#include <stdio.h>
#include <vector>
#include <unordered_set>
#include "hashtable.h"
#include "pqobject.h"
using namespace std;

// class Node{
//     public:
//     const double dist;
//     const int index;
//     Node(int index,double dist):index(index),dist(dist){};
// };

class Graph{
    private:
    vector<vector<PQObject>> neighbours;
    public:
    Graph(const vector<vector<int>> &points,const int L,const int K,const int K_N);
    priority_queue<PQObject> search(const vector<int> &query);
};
#endif