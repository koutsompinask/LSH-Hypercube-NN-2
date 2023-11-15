#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <vector>
#include <random>
#define DIM 784
#define WIN 4000
#define M_NUM 0xFFA
using namespace std;

class HTObject{
    private:
    public:
        const uint index;
        const uint id;
        const vector<int> p;
        HTObject(int id,vector<int> p,uint index):id(id),p(p),index(index){};
        void print(){
            cout << " id : " << id << endl;
            for (int i=0;i<p.size();i++) {
                cout << p[i] << ",";
                if ((i+1)%28==0) cout << endl;
            }
        }
};

class HashTable{
    private:
        vector<vector<HTObject>> table;
        vector<vector<double>> v_vects;
        vector<uint> rs;
        vector<double> ts;
        const int K_DIM;
        const int TABLESIZE;
        uint g(vector<int> p);
        uint g(vector<double> p);
        int hash(vector<int> p){return g(p)%TABLESIZE;};
        int hash(vector<double> p){return g(p)%TABLESIZE;};
    public:
        vector<HTObject> getBucket(vector<int> p){return table[hash(p)];}; //get corresponding bucket for int and double vector
        vector<HTObject> getBucket(vector<double> p){return table[hash(p)];};
        uint id(vector<int> p){return g(p)%M_NUM;};
        HashTable(int K_DIM,int TABLESIZE);
        void place(vector<int> p,int index);
        void print();
};

#endif