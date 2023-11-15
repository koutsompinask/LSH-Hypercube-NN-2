#ifndef GNNS_H
#define GNNS_H
#include <stdio.h>
#include <unordered_set>
#include "graph.h"
using namespace std;

class Gnns : public Graph{
    public:
    Gnns(const vector<vector<int>> &points,const int L,const int K,const int K_N);
    priority_queue<PQObject> search(const vector<int> &query) override;
};
#endif