#include <stdio.h>
#include <chrono>
#include <queue>
#include "pqobject.h"
#include "gnns.h"
#include "helper.h"

int main(){
    vector<vector<int>> photos=readInput("input.dat",5000);
    Graph *g; 
    Gnns gnns = Gnns(photos,5,4,50);
    g=&gnns;
    vector<vector<int>> queries = readQuery("query.dat",1);
    priority_queue<PQObject> S = g->search(queries[0]);
    priority_queue<PQObject> pq_real;
    int i=0;
    while (!S.empty() && i< 3 ){
        PQObject pqo = S.top();
        cout << "Nearest Neighbour " << i+1 << " : " << pqo.getIndex() <<" " << pqo.getDistance() << endl;
        S.pop();
        i++;
    }
    auto startTrue = chrono::high_resolution_clock::now();
    for (int i=0;i<photos.size();i++){
        vector<int> obj=photos[i];
        PQObject pqo(dist(queries[0],obj),obj,i);
        pq_real.push(pqo);
    }
    auto endTrue = chrono::high_resolution_clock::now();
    auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);
    i = 0;
    while (!pq_real.empty() && i< 3 ){
        PQObject pqo_real = pq_real.top();
        cout << "Real Distance :" << pqo_real.getDistance() << endl;
        pq_real.pop();
        i++;
    }
    return 0;
}