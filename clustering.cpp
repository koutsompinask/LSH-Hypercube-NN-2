#include <iostream>
#include <vector>
#include <time.h>
#include <bits/stdc++.h>
#include "helper.h"
#include "clusteringMethods.h"
#define K_DEF 10
#define L_DEF 3
#define K_LSH_DEF 4
#define M_DEF 10
#define PROBES_DEF 2
#define K_HCUBE_DEF 3

using namespace std;

int main(int argc,char *argv[]){
    //read arguments and assign given values
    map<string,string> args=getArguments(argc,argv);
    string method = (args.count("m")==1) ? args["m"] : "CLASSIC";
    transform(method.begin(), method.end(), method.begin(), ::toupper);
    const string configFile = (args.count("c")==1) ? args["c"] : "cluster.conf";
    string outputFile = (args.count("o")==1) ? args["o"] : "";
    string inputFile = (args.count("i")==1) ? args["i"] : "input.dat";
    string inputEncFile = "x_train_enc.txt";
    string queryEncFile = "x_test_enc.txt";
    const bool complete = args.count("complete")==1;
    const bool full = args.count("full")==1;
    //read config properties
    map<string,int> confMap = getClusterConf(configFile);
    const int K = (confMap.count("K")==1) ? confMap["K"] : K_DEF;
    const int L = (confMap.count("L")==1) ? confMap["L"] : L_DEF;
    const int K_LSH = (confMap.count("K_LSH")==1) ? confMap["K_LSH"] : K_LSH_DEF;
    const int K_HCUBE = (confMap.count("K_HCUBE")==1) ? confMap["K_HCUBE"] : K_HCUBE_DEF;
    const int M = (confMap.count("M")==1) ? confMap["M"] : M_DEF;
    const int PROBES = (confMap.count("PROBES")==1) ? confMap["PROBES"] : PROBES_DEF;
    
    int datasize;
    fprintf(stdout,"how many data do you want to use (up to 60000) ?\n");
    cin >> datasize;

    srand(time(NULL));//seed for rand
    vector<vector<int>> photos=readInput(inputFile,datasize);//read part of input file (to slow with whole dataset)
    vector<vector<int>> photosEnc=readEncoded(inputEncFile,datasize);
    vector<vector<int>> intCentroids=init(photosEnc,K); //init++
    vector<vector<double>> centroids(intCentroids.size()); 
    //cast centroids to double numbers
    for (int i=0 ; i < intCentroids.size() ; i++){
        for (int j = 0 ; j < intCentroids[0].size() ; j++){
            centroids[i].push_back((double)intCentroids[i][j]);
        }
    }
    cout << "finished init++"<< endl;

    if (args.count("o")==0){
        fprintf(stdout,"Please provide name of output file (Give \"d\" to continue with \"output.txt\")\n");
        cin >> outputFile;
        if (outputFile=="d") outputFile = "output.txt";
    }

    ofstream output ;
    output=ofstream(outputFile);
    if (output.is_open()) {
        cout.rdbuf(output.rdbuf());
    }

    vector<unordered_set<int>> clusterAssignment(centroids.size());
    vector<int> belongsTo(photos.size(),-1);
    //call clustering algorithm based on -m parameter
    if (method.compare("CLASSIC")==0) {
        lloyds(photosEnc,centroids,belongsTo,clusterAssignment,complete);
    } else if (method.compare("LSH")==0) {
        lshRange(photosEnc,centroids,belongsTo,clusterAssignment,K_LSH,L,complete);
    } else if (method.compare("HYPERCUBE")==0) {
        cubeRange(photosEnc,centroids,belongsTo,clusterAssignment,K_HCUBE,M,PROBES,complete);
    } else {
        cout << "please give parameter -m [lsh,hypercube or classic]\n"; //wrong method given
        exit(-1);
    }
    vector<double> s = silhouette(photosEnc,centroids,clusterAssignment,belongsTo);
    if (output.is_open()) output.close();
    return 0;
}