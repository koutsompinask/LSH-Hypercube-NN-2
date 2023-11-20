#include <stdio.h>
#include <queue>
#include <omp.h>
#include <fstream>
#include "gnns.h"
#include "hashtable.h"
#include "helper.h"

Gnns::Gnns(const vector<vector<int>> &points,int L,int K_DIM,int K_N,int E,int R):Graph(points),E(E),R(R){
    //if (!this->readFromFile()){
    HashTable* ht[L];
    #pragma omp parallel for
    for (int i=0;i<L;i++){
        ht[i]=new HashTable(K_DIM,points.size()/128);
        for (int j=0;j<points.size();j++){
            ht[i]->place(points[j],j);
        }
    }
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
    //}
}

priority_queue<PQObject> Gnns::search(const vector<int> &query,chrono::microseconds &time){
    auto start = chrono::high_resolution_clock::now();
    priority_queue<PQObject> S;
    unordered_set<int> indices;
    for (int i=0; i < R ; i++){
        bool cond=true;
        double yDist=-1;
        int y=rand()%neighbours.size();
        while(cond){
            double min=-1;
            int minInd=-1;
            int counter=0;
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
                counter++;
                if (counter>=E) break; 
            }
            if( min < yDist || yDist==-1){
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

void Gnns::writeToFile(){
    ofstream file("gnns.txt");
    vector<vector<PQObject>> ;

    if (!file.is_open()) {
        cerr << "cant write to file";
        return;
    }
    //TODO write neighbours 
    //idea write PQOBJECT per line
    //seperate vectors by special delim like "NEXT\n"
    //add like "END" to know where to stop
    file.close();
}

bool Gnns::readFromFile(){
    ifstream file("gnns.txt");
    vector<vector<PQObject>> ;

    if (!file.is_open()) {
        return false;//file not found so we havent saved it
    }

    //TODO read neighbours

    file.close();
    return true;
}