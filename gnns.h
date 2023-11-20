#ifndef GNNS_H
#define GNNS_H
#include <stdio.h>
#include <unordered_set>
#include "graph.h"
using namespace std;

class Gnns : public Graph{
    const int E;
    const int R;
    public:
    Gnns(const vector<vector<int>> &points,int L,int K_DIM,int K_N,int E,int R);
    priority_queue<PQObject> search(const vector<int> &query,chrono::microseconds &time) override;
};
#endif