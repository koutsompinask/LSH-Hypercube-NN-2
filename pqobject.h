#ifndef PQOBJECT_H
#define PQOBJECT_H

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

//pq object to store objects in primary queue and compare them based on dist
class PQObject{
    private:
        double distance;
        int index;
        vector<int> p;
    public:
        PQObject(double distance, vector<int> p,int index):distance(distance), p(p), index(index){};
        PQObject(const PQObject& other) {
            distance=other.distance;
            index=other.index;
            p=other.p;
        }
        vector<int> getVector(){return p;};
        double getDistance(){return distance;};
        int getIndex(){return index;};
        bool operator<(const PQObject& point) const{
            return distance > point.distance;
        }
};

#endif