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
    public:
    Mrng(const vector<vector<int>> &points,const int L,const int K,const int K_N);
    priority_queue<PQObject> search(const vector<int> &query) override;
};
#endif