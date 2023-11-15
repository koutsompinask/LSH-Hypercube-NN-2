#include <stdio.h>
#include <queue>
#include "mrng.h"
#include "hashtable.h"
#include "helper.h"

Mrng::Mrng(const vector<vector<int>> &points,const int L,const int K,const int K_N):Graph(points){
    HashTable* ht[L];
    for (int i=0;i<L;i++){
        ht[i]=new HashTable(K,points.size()/128);
        for (int j;j<points.size();j++){
            ht[i]->place(points[j],j);
        }
    }
    for (int i;i<points.size();i++){
        unordered_set<int> LpInd;
        unordered_set<int> indices;
        double min=-1;
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
        neighbours[i].push_back(PQObject(min,points[minInd],minInd));
        LpInd.insert(minInd);
        LpInd.insert(i);
        for(int j=0;j<points.size();j++){
            bool notLongest=true;
            if (LpInd.count(j)>0) continue; //check if j is in Lp
            double pr = dist(points[i],points[j]);
            for(auto t: neighbours[i]){ //for each t
                double pt = t.getDistance();
                double rt = dist(t.getVector(),points[j]);
                if (pr>pr || pr>rt) { //if pr the longest in prt we shouldnt add it
                    notLongest=false;
                    break;
                }
            }
            if (notLongest){ //if its not the longest in all triangles add it to Lp
                neighbours[i].push_back(PQObject(pr,points[j],j));
                LpInd.insert(j);
            }
        }
    }
    //find mean of all points
    vector<double> mean(points[0].size(),0);
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
    navigationNode=minInd;
    for (int i=0;i<L;i++){
        delete(ht[i]);
    }
}

priority_queue<PQObject> Mrng::search(const vector<int> &query){
    priority_queue<PQObject> S;
    priortiy_queue<PQObject> Checked;
    int q = navigationNode;
    S.push(PQObject(dist(points),u.getVector(),u.getIndex()));
    for (int i=0; i < 10 ; i++){
        double min=-1;
        int minInd=-1;
        for (auto t:neighbours[q]){
            double d = dist(t.getVector(),query);
            if (d<min || min<0){
                min=d;
                minInd=t.getIndex();
            }
        }
        q=minInd;
    }
    return S;
}