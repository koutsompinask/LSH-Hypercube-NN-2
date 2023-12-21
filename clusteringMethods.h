#ifndef CLUSTERMETHODS_H
#define CLUSTERMETHODS_H
#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
using namespace std;

map<string, int> getClusterConf(string filename);
vector<vector<int>> init(vector<vector<int>> photos,int k);
void lloyds(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo , vector<unordered_set<int>> &clusterAssignment,bool complete);
void lshRange(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo ,vector<unordered_set<int>> &clusterAssignment,const int K_LSH,const int L,bool complete);
void cubeRange(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo ,vector<unordered_set<int>> &clusterAssignment,const int K_HCUBE,const int M,const int PROBES,bool complete);
vector<double> silhouette(const vector<vector<int>> &photos, const vector<vector<double>> &centroids , const vector<unordered_set<int>> &clusterAssignment,const vector<int> &belongsTo);
vector<vector<double>> turnCentroidToNearest(vector<vector<double>> &centroids , vector<vector<int>> &pointsEnc , vector<vector<int>> &points);
#endif