#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include "pqobject.h"
using namespace std;

class Graph {
    protected:
    vector<vector<PQObject>> neighbours;
    public:
    Graph(const vector<vector<int>> &points):neighbours(points.size()){};
    virtual ~Graph(){};
    virtual priority_queue<PQObject> search(const vector<int> &query)=0;
};
#endif