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
    const int l;
    vector<int> navigationVector;
    public:
    Mrng(const vector<vector<int>> &points,const int l);
    priority_queue<PQObject> search(const vector<int> &query,chrono::microseconds &time) override;
};
#endif