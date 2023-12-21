#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#define DIM 784
using namespace std;

map<string, string> getArguments(int argc, char*argv[]){
    map<string, string> clArguments;
    for (int i=1; i<argc; i++){
        string key=argv[i];
        if (key[0]!='-') continue; //not a flag
        key.erase(key.begin());
        string value= (i+1<argc) ? argv[i+1] : "";
        clArguments[key]=value;
    }
    return clArguments;
}


double dist(const vector<int> &p,const vector<int> &q){
    if(p.size() != q.size()) {
        cerr << p.size() << " " << q.size() <<"error in dist (different vector sizes)\n";
        exit(-1);
    }
    double dist=0.0;
    for (int i=0;i<p.size();i++){
        double dif = p[i]-q[i];
        dist+=dif*dif;
    }
    return sqrt(dist);
}

double dist(const vector<int> &p,const vector<double> &q){
    if(p.size() != q.size()) {
        cerr << p.size() << " " << q.size() <<"error in dist (different vector sizes)\n";
        exit(-1);
    }
    double dist=0.0;
    for (int i=0;i<p.size();i++){
        double dif = p[i]-q[i];
        dist+=dif*dif;
    }
    return sqrt(dist);
}

double dist(const vector<double> &p,const vector<double> &q){
    if(p.size() != q.size()) {
        cerr << p.size() << " " << q.size() <<"error in dist (different vector sizes)\n";
        exit(-1);
    }
    double dist=0.0;
    for (int i=0;i<p.size();i++){
        double dif = p[i]-q[i];
        dist+=dif*dif;
    }
    return sqrt(dist);
}

vector<vector<int>> readInput(string filename,int limit=0){
    FILE* file = fopen(filename.c_str(), "rb"); // Open the file in binary mode
    if (!file) {
        cerr << "Failed to open the file: " << filename << endl;
        exit(-1); // Return an empty vector if the file can't be opened
    }

    vector<vector<int>> bytes;
    unsigned char byte[DIM];
    int i=0,j=0;
    bool clear=true;
    int x=fread(&byte, sizeof(unsigned char), 16, file);
    if (x<16) exit(-1);//error
    while (!feof(file)) {
        //read photo
        x=fread(&byte, sizeof(unsigned char), DIM, file);
        if(x<0){
            cerr << "error in reading byte" << endl;
            exit(-1);
        }
        else if (x<DIM) break;
        i++;
        //have read first 16 bytes (not data)
        vector<int> v;
        for (int k=0;k<DIM;k++){
            v.push_back(int(byte[k]));
        }
        bytes.push_back(v);
        if (limit!=0 && i==limit) break;
    }
    fclose(file);
    return bytes;
}

vector<vector<int>> readQuery(string filename,int limit){
    FILE* file = fopen(filename.c_str(), "rb"); // Open the file in binary mode
    if (!file) {
        cerr << "Failed to open the file: " << filename << endl;
        exit(-1); // Return an empty vector if the file can't be opened
    }
    vector<vector<int>> bytes;
    unsigned char byte[DIM];
    bool clear=true;
    int x=fread(&byte, sizeof(unsigned char), 16, file);
    int i=0;
    if (x<16) exit(-1);//error
    while (!feof(file) && i<limit) {
        //read photo
        x=fread(&byte, sizeof(unsigned char), DIM, file);
        if(x<0){
            cerr << "error in reading byte" << endl;
            exit(-1);
        }
        else if (x<DIM) break;
        //have read first 16 bytes (not data)
        vector<int> v;
        for (int k=0;k<DIM;k++){
            v.push_back(int(byte[k]));
        }
        bytes.push_back(v);
        i++;
    }
    fclose(file);
    return bytes;
}

vector<vector<int>> readEncoded(string filename,int limit=0){
    // Open the file
    ifstream infile(filename);

    // Check if the file is open
    if (!infile.is_open()) {
        cerr << "Error opening file" << endl;
        exit(-1);
    }

    vector<vector<int>> encodedData;
    int value;
    string line;
    int i=0;
    while (getline(infile, line)) {
        istringstream iss(line);
        vector<int> row;

        // Read values from the line and push them into the row vector
        while (iss >> value) {
            row.push_back(value);
        }
        encodedData.push_back(row);
        i++;
        if (limit!=0 && i==limit) break;
    }

    // Close the file
    infile.close();

    return encodedData;
}

void printImage(vector<int> query){
    for (int i=0; i<DIM ;i++){
        if (query[i]==0) cout << " ";
        else cout << "0" ;
        if ((i+1)%28==0) cout <<endl;
    }
}

int bSearch(vector<double> p, int l, int r, double x){
    if (l<=r){
        int mid=(l+r)/2;
        if ( (p[mid-1] < x) && (p[mid] >= x)){
            return mid;
        }
        if (p[mid-1] >= x ){
            return bSearch(p, l, mid-1, x);
        }
        if (p[mid] < x ){
            return bSearch(p, mid+1, r, x);
        }
    }
    exit(-1);//something went wrong
}