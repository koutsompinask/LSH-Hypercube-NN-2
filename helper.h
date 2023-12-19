#ifndef HELPER_H
#define HELPER_H
#include <vector>
#include <string>
#include <map>
using namespace std;

map<string, string> getArguments(int argc, char* argv[]);
double dist(const vector<int> &p,const vector<int> &q);
double dist(const vector<int> &p,const vector<double> &q);
double dist(const vector<double> &p,const vector<double> &q);
vector<vector<int>> readInput(string filename,int limit=0);
vector<vector<int>> readQuery(string filename,int index);
vector<vector<int>> readEncoded(string filename,int limit=0);
void printImage(vector<int> image);
int bSearch(vector<double>, int, int, double);

#endif