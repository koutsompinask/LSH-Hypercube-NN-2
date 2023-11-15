#ifndef MRNG_H
#define MRNG_H
#include <stdio.h>
#include <unordered_set>
#include "graph.h"
#include "pqobject.h"
using namespace std;

class Mrng : public Graph{
    private:
    int navigationNode;
    vector<int> navigationVector;
    public:
    Mrng(const vector<vector<int>> &points);
    priority_queue<PQObject> search(const vector<int> &query) override;
};
#endif