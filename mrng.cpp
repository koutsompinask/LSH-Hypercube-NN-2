#include <stdio.h>
#include <queue>
#include <chrono>
#include <omp.h>
#define L 5
#define K 5
#include "mrng.h"
#include "hashtable.h"
#include "helper.h"

Mrng::Mrng(const vector<vector<int>> &points,const int l):Graph(points),l(l){
    auto start = chrono::high_resolution_clock::now();
    //use lsh to find nearest neighbour for each node , and for finding navigation node
    HashTable* ht[L];
    #pragma omp parallel for
    for (int i=0;i<L;i++){
        ht[i]=new HashTable(K,points.size()/128+1);
        for (int j=0;j<points.size();j++){
            ht[i]->place(points[j],j);
        }
    }
    #pragma omp parallel for
    for (int i=0;i<points.size();i++){
        unordered_set<int> LpInd; 
        unordered_set<int> indices;
        double min=-1;//find nearest neighbour with lsh
        int minInd=-1;
        for (auto h : ht){
            for (auto v : h->getBucket(points[i])){
                if (v.index==i) continue;
                if (indices.count(v.index)>0) continue;
                double d=dist(v.p,points[i]);
                if (min==-1 || d<min){
                    min = d;
                    minInd = v.index;
                }
                indices.insert(v.index);
            }
        }
        neighbours[i].push_back(PQObject(min,points[minInd],minInd)); //add nearest neighbour
        LpInd.insert(minInd);
        LpInd.insert(i); //add them to lp
        for(int j=0;j<points.size();j++){ //for every point
            if (LpInd.count(j)>0) continue; //check if j is in Lp
            bool notLongest=true; 
            double pr = dist(points[i],points[j]);
            for(auto t: neighbours[i]){ //for each t
                double pt = t.getDistance();
                double rt = dist(t.getVector(),points[j]);
                if (pr>pr || pr>rt) { //if pr the longest in prt we shouldnt add it
                    notLongest=false;
                    break;
                }
            }
            if (notLongest){ //if its not the longest in all triangles add it to Lp else continue to next point
                neighbours[i].push_back(PQObject(pr,points[j],j));
                LpInd.insert(j);
            }
        }
    }
    //find mean of all points
    vector<double> mean(points[0].size(),0.0);
    for (auto p:points){
        for (int i=0;i<mean.size();i++){
            mean[i]+=double(p[i])/points.size();
        }
    }
    //find closest to point to mean using lsh
    double min=-1;
    int minInd=-1;
    unordered_set<int> indices;
    for (auto h : ht){
        for (auto v : h->getBucket(mean)){
            if(indices.count(v.index)>0) continue;
            double d=dist(v.p,mean);
            if (min==-1 || d<min){
                min = d;
                minInd = v.index;
            }
            indices.insert(v.index);
        }
    }
    //save navigation node
    navigationNode=minInd;
    navigationVector=points[navigationNode];
    #pragma omp parallel for
    for (int i=0;i<L;i++){
        delete(ht[i]);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << double(duration.count()/1e6) << endl;
}

priority_queue<PQObject> Mrng::search(const vector<int> &query,chrono::microseconds &time){
    auto start = chrono::high_resolution_clock::now();
    priority_queue<PQObject> R;
    R.push(PQObject(dist(navigationVector,query),navigationVector,navigationNode));
    unordered_set<int> indices;
    //initialize R set using a pq to keep them sorted
    priority_queue<PQObject> checked;
    //another for checked nodes
    for (int i=1; i < l ;){
        PQObject q = R.top();
        R.pop();
        if (indices.count(q.getIndex())>0) continue; //if already checked
        indices.insert(q.getIndex());
        checked.push(q); // check first node
        for (auto t:neighbours[q.getIndex()]){
            double d = dist(t.getVector(),query);
            R.push(PQObject(d,t.getVector(),t.getIndex()));
        }
        i++;
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(end - start);
    time+=duration;
    //merge queues 
    while(!R.empty()){
        PQObject pqo = R.top();
        if(indices.count(pqo.getIndex())==0){
            checked.push(R.top());
        }
        R.pop();
    }
    return checked;
}