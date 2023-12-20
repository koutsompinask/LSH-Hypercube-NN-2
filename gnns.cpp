#include <stdio.h>
#include <queue>
#include <omp.h>
#include <fstream>
#include "gnns.h"
#include "hashtable.h"
#include "helper.h"

Gnns::Gnns(const vector<vector<int>> &points,int L,int K_DIM,int K_N,int E,int R):Graph(points),E(E),R(R){
    auto start = chrono::high_resolution_clock::now();
    HashTable* ht[L];
    #pragma omp parallel for // for parallelism
    for (int i=0;i<L;i++){
        ht[i]=new HashTable(K_DIM,points.size()/128+1,points[0].size());
        for (int j=0;j<points.size();j++){
            ht[i]->place(points[j],j);
        }
    }
    //use lsh to find nearest neighbours for each node for graph construction
    #pragma omp parallel for
    for (int i=0;i<points.size();i++){
        priority_queue<PQObject> pq;
        unordered_set<int> indices;
        for (auto h : ht){
            for (auto v : h->getBucket(points[i])){
                if (v.index==i || indices.count(v.index)>0) continue;
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
    #pragma omp parallel for
    for (int i=0;i<L;i++){
        delete(ht[i]);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << double(duration.count()/1e6) << endl;
}

priority_queue<PQObject> Gnns::search(const vector<int> &query,chrono::microseconds &time){
    auto start = chrono::high_resolution_clock::now();
    priority_queue<PQObject> S; //for storing nodes and return to main
    unordered_set<int> indices; //avoid doublechecking
    for (int i=0; i < R ; i++){ //random restarts
        bool cond=true;
        double yDist=-1;
        int y=rand()%neighbours.size(); //random start node
        while(cond){
            double min=-1;
            int minInd=-1;
            int counter=0;
            for (auto u : neighbours[y]){ //iterate through y's neighbours
                double d = dist(u.getVector(),query); //find neighbour closest to query
                if (min==-1 || d < min){
                    min=d;
                    minInd=u.getIndex();
                }
                if (indices.count(u.getIndex())==0){ //add it to priority queue if its not there
                    S.push(PQObject(d,u.getVector(),u.getIndex()));
                    indices.insert(u.getIndex());
                }
                counter++;
                if (counter>=E) break; //stop if we checked E neighbours
            }
            if( min < yDist || yDist==-1){//if found neighbour closer to query update and continue
                y=minInd;
                yDist=min;
            } else {
                cond=false; //reached local min
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(end - start);
    time+=duration;
    return S;
}
