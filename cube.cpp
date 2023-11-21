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

    if (args.count("d")==0){
        fprintf(stdout,"Please provide name of dataset file (Give \"def\" to continue with \"input.dat\")\n");
        cin >> inputFile;
        if (inputFile=="def") inputFile = "input.dat";
    }
    
    //init cube
    HyperCube hc(D);
    vector<vector<int>> photos=readInput(inputFile,5000);  
    

    //place data objects
    for (int i=0;i<photos.size();i++){
        hc.place(photos[i],i);
    }

    cout << "finished training\n";

    int loops;
    string contFlag;

    if (args.count("q")==0){
        fprintf(stdout,"Please provide name of query file (Give \"def\" to continue with \"query.dat\")\n");
        cin >> queryFile;
        cout << "query |" << queryFile << "|\n";
        if (queryFile=="def") queryFile = "query.dat";
    }
    fprintf(stdout,"how many queries do you want to execute ?\n");
    cin >> loops;
    vector<vector<int>> queries = readQuery(queryFile,loops);  //read as many queries as given from user

    if (args.count("o")==0){
        fprintf(stdout,"Please provide name of output file (Give \"def\" to continue with \"output.txt\")\n");
        cin >> outputFile;
        if (outputFile=="def") outputFile = "output.txt";
    }

    ofstream output ;
    output=ofstream(outputFile);
    if (output.is_open()) {
        cout.rdbuf(output.rdbuf());
    }

    while(queries.size()!=0){
        double maf=-1;
        cout << "START OF SEARCH \n\n";
        std::chrono::microseconds avgReal(0),avgApprox(0);
        for (int k=0;k<queries.size();k++){
            //2 priority queues for storing objects and distances 
            //one for heuristic search and one for brute force
            priority_queue<PQObject> pq;
            priority_queue<PQObject> pq_real;
            vector<int> query=queries[k];
            cout << "Query: " << k <<endl;

            auto startLsh = chrono::high_resolution_clock::now();
            vector<HCObject> bucket = hc.getBucket(query,PROBES,M); //get bucket
            for (HCObject obj:bucket){//for each object in same bucket
                PQObject pqo(dist(query,obj.p),obj.p,obj.index); //create and push priority queue object
                pq.push(pqo);
            }
            auto endLsh = chrono::high_resolution_clock::now();
            auto durationLsh = chrono::duration_cast<std::chrono::microseconds>(endLsh - startLsh);
            avgApprox+=durationLsh;

            auto startTrue = chrono::high_resolution_clock::now();
            for (int i=0;i<photos.size();i++){ //for every photo
                PQObject pqo(dist(query,photos[i]),photos[i],i);
                pq_real.push(pqo);
            }
            auto endTrue = chrono::high_resolution_clock::now();
            auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);
            avgReal+=durationTrue;

            //print results
            int i = 0;
            while (!pq.empty() && i< K_N ){
                PQObject pqo = pq.top();
                PQObject pqo_real = pq_real.top();
                cout << "Nearest Neighbour " << i+1 << " : " <<  pqo.getIndex() << endl;
                cout << "Distance :" << pqo.getDistance() << endl;
                cout << "Real Distance :" << pqo_real.getDistance() << endl;
                double af=pqo.getDistance()/pqo_real.getDistance();
                if(af>maf) maf=af;
                pq.pop();
                pq_real.pop();
                i++;
            }
            cout << "tLsh: " << double(durationLsh.count()/1e6) << " seconds" << endl;
            cout << "tReal: " << double(durationTrue.count()/1e6) << " seconds" << endl;

            cout << "Range Search :\n";
            int l=0;
            for (HCObject obj:bucket){ //bucket retrieved earlier
                double d = dist(obj.p,query);
                if (d < R) {
                    cout << "Index " << obj.index << " distance " << d << endl;
                    l++;
                }
                if (l>=20) break; //if enough points printed , break
            }
            cout << endl;
            fflush(stdout);
        }
        cout << "tAverageApproximate: " << double((avgApprox/queries.size()).count()/1e6) << endl;
        cout << "tAverageReal: " << double((avgReal/queries.size()).count()/1e6) << endl;
        cout << "MAF: " << maf << endl;
        queries.clear();
        fprintf(stdout,"Do you want to continue with another query ?(y/n)\n");
        cin >> contFlag;
        if (contFlag[0]!='y') break; //if not 'y' provided , stop the loop
        fprintf(stdout,"Please provide name of query file (Give \"def\" to continue with previous file\n");
        string prev = queryFile;
        cin >> queryFile;
        if (queryFile=="def") queryFile = prev;
        fprintf(stdout,"how many queries do you want to execute ?\n");
        cin >> loops;
        queries = readQuery(queryFile,loops); //read as many queries as given from user
    }

    if (output.is_open()) output.close();

    return 0;
}