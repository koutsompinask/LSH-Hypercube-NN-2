#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <iostream>
#include <vector>
#include <random>
#include <map>
#define DIM 784
#define HP_WIN 4000
#define M_NUM 0xFFA
using namespace std;

class HCObject{
    private:
    public:
        const uint index;
        const vector<int> p;
        HCObject(vector<int> p,uint index):p(p),index(index){};
        void print(){
            for (int i=0;i<p.size();i++) {
                cout << p[i] << ",";
                if ((i+1)%28==0) cout << endl;
            }
        }
};

class HyperCube{
    private:
        vector<vector<HCObject>> table;
        vector<vector<double>> v_vects;
        vector<uint> rs;
        vector<double> ts;
        const int D_DIM;
        int hash(vector<int> p);
        int hash(vector<double> p);
        vector<int> getNprobes(int maxDif, string number,vector<int> vectorSoFar , int bitSoFar);
        vector<map<uint,int>> f;
        vector<HCObject> getBucketHelper(int h,int probes,int maxPoints);
    public:
        vector<HCObject> getBucket(vector<int> p,int probes,int maxPoints){return getBucketHelper(hash(p),probes,maxPoints);}; //call helper function both for int and doubles
        vector<HCObject> getBucket(vector<double> p,int probes,int maxPoints){return getBucketHelper(hash(p),probes,maxPoints);};
        HyperCube(int d);
        void place(vector<int> p,int index);
        void print();
};

#endif