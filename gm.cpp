#include <stdio.h>
#include <queue>
#include "gnns.h"
#include "hashtable.h"
#include "helper.h"

Graph::Graph(const vector<vector<int>> &points,const int L,const int K,const int K_N):neighbours(points.size()){
    HashTable* ht[L];
    for (int i=0;i<L;i++){
        ht[i]=new HashTable(K,points.size()/128);
        for (int j;j<points.size();j++){
            ht[i]->place(points[j],j);
        }
    }
    for (int i;i<points.size();i++){
        priority_queue<PQObject> pq;
        unordered_set<int> indices;
        for (auto h : ht){
            for (auto v : h->getBucket(points[i])){
                if (indices.count(v.index)>0) continue;
                pq.push(PQObject(dist(points[i],v.p),v.p,v.index));
                indices.insert(v.index);
            }
        }
        int j = 0;
        while (!pq.empty() && j< K_N ){
            neighbours[i].push_back(pq.top());
            pq.pop();
            j++;
        }
    }
    for (int i=0;i<L;i++){
        delete(ht[i]);
    }
}

priority_queue<PQObject> Graph::search(const vector<int> &query){
    priority_queue<PQObject> S;
    unordered_set<int> indices;
    for (int i=0; i < 10 ; i++){
        int y=rand()%neighbours.size();
        for (int t=1; t<100 ; t++){
            int min=-1;
            int minInd=-1;
            for (auto u : neighbours[y]){
                double d = dist(u.getVector(),query);
                if (min==-1 || d < min){
                    min=d;
                    minInd=u.getIndex();
                }
                if (indices.count(u.getIndex())==0){
                    S.push(PQObject(d,u.getVector(),u.getIndex()));
                    indices.insert(u.getIndex());
                }
            }
            y=minInd;
            if (y<0) break;
        }
    }
    return S;
}