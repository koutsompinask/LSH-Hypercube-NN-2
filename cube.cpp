#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <map>
#include <string>
#include <unordered_set>
#include <chrono>
#define D_DEF 14
#define M_DEF 10
#define K_N_DEF 1
#define K_DIM_DEF 4
#define R_DEF 10000
#define PROBES_DEF 2
using namespace std;
#include "helper.h"
#include "hypercube.h"
#include "pqobject.h"

int main(int argc, char* argv[]){
    //read args
    map<string,string> args=getArguments(argc,argv);
    const int D = (args.count("k")==1) ? stoi(args["k"]) : D_DEF;
    const int M = (args.count("M")==1) ? stoi(args["M"]) : M_DEF;
    const int K_N = (args.count("N")==1) ? stoi(args["N"]) : K_N_DEF;
    const int R = (args.count("R")==1) ? stoi(args["R"]) : R_DEF;
    const int PROBES = (args.count("PROBES")==1) ? stoi(args["PROBES"]) : PROBES_DEF;
    const bool skipId = (args.count("id")==0);
    string inputFile = (args.count("d")==1) ? args["d"] : "input.dat";
    string queryFile = (args.count("q")==1) ? args["q"] : "query.dat";
    string outputFile = (args.count("o")==1) ? args["o"] : "";
    string inputEncFile = (args.count("de")==1) ? args["de"] : "x_train_enc4.txt";
    string queryEncFile = (args.count("qe")==1) ? args["qe"] : "x_test_enc4.txt";
    int datasize;
    fprintf(stdout,"how many data do you want to use (up to 60000) ?\n");
    cin >> datasize;
    //init cube
    vector<vector<int>> photos=readInput(inputFile,datasize);  
    vector<vector<int>> photosEnc = readEncoded(inputEncFile,datasize);
    //vector<vector<int>> photosEnc = photos;

    HyperCube hc(D,photosEnc[0].size());

    //place data objects
    for (int i=0;i<photos.size();i++){
        hc.place(photosEnc[i],i);
    }

    cout << "finished training\n";

    int loops;
    string contFlag;

    fprintf(stdout,"how many queries do you want to execute ?\n");
    cin >> loops;
    vector<vector<int>> queries = readQuery(queryFile,loops);  //read as many queries as given from user
    vector<vector<int>> queriesEnc = readEncoded(queryEncFile,loops);
    //vector<vector<int>> queriesEnc = queries;

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

    double af=0,maf=0;
    cout << "START OF SEARCH \n\n";
    std::chrono::microseconds avgReal(0),avgApprox(0); 
    for (int k=0;k<queries.size();k++){
        //2 priority queues for storing objects and distances 
        //one for heuristic search and one for brute force
        priority_queue<PQObject> pq;
        priority_queue<PQObject> pq_real;
        vector<int> query=queries[k];
        vector<int> queryEnc=queriesEnc[k];
        cout << "Query: " << k <<endl;

        auto startLsh = chrono::high_resolution_clock::now();
        vector<HCObject> bucket = hc.getBucket(queryEnc,PROBES,M); //get bucket
        for (HCObject obj:bucket){//for each object in same bucket
            PQObject pqo(dist(queryEnc,obj.p),obj.p,obj.index); //create and push priority queue object
            pq.push(pqo);
        }
        auto endLsh = chrono::high_resolution_clock::now();
        auto durationLsh = chrono::duration_cast<std::chrono::microseconds>(endLsh - startLsh);

        auto startTrue = chrono::high_resolution_clock::now();
        for (int i=0;i<photos.size();i++){ //for every photo
            PQObject pqo(dist(query,photos[i]),photos[i],i);
            pq_real.push(pqo);
        }
        auto endTrue = chrono::high_resolution_clock::now();
        auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);

        //print reesults
        int i = 0;
        while (!pq.empty() && i< K_N ){
            PQObject pqo = pq.top();
            PQObject pqo_real = pq_real.top();
            double d = dist(query,photos[pqo.getIndex()]);
            cout << "Nearest Neighbour " << i+1 << " : " <<  pqo.getIndex() << endl;
            cout << "Distance :" << d << endl;
            cout << "Real Distance :" << pqo_real.getDistance() << endl;
            double x = d/pqo_real.getDistance();
            if (x > maf){
                maf = x;
            }
            af += x/queriesEnc.size();
            pq.pop();
            pq_real.pop();
            i++;
        }
        avgReal+=durationTrue/queries.size();
        avgApprox+=durationLsh/queries.size();
        fflush(stdout);
    }
    cout << "tAverageApproximate: " << double((avgApprox).count()/1e6) << endl;
    cout << "tAverageReal: " << double((avgReal).count()/1e6) << endl;
    cout << "MAF: " << maf << endl;
    cout << "avgAF: " << af << endl;

    if (output.is_open()) output.close();

    return 0;
}