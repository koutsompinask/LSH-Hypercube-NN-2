#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <map>
#include <string>
#include <unordered_set>
#include <chrono>
#define L_DEF 5
#define K_N_DEF 1
#define K_DIM_DEF 4
using namespace std;
#include "helper.h"
#include "hashtable.h"
#include "pqobject.h"

int main(int argc, char* argv[]){
    //read args
    map<string,string> args=getArguments(argc,argv);
    const int L = (args.count("L")==1) ? stoi(args["L"]) : L_DEF;
    const int K_DIM = (args.count("k")==1) ? stoi(args["k"]) : K_DIM_DEF;
    const int K_N = (args.count("N")==1) ? stoi(args["N"]) : K_N_DEF;
    string outputFile = (args.count("o")==1) ? args["o"] : "output.txt";
    string inputFile = (args.count("d")==1) ? args["d"] : "input.dat";
    string queryFile = (args.count("q")==1) ? args["q"] : "query.dat";
    string inputEncFile = "x_train_enc32.txt";
    string queryEncFile = "x_test_enc32.txt";
    int datasize;
    fprintf(stdout,"how many data do you want to use (up to 60000) ?\n");
    cin >> datasize;

    vector<vector<int>> photos=readInput(inputFile,datasize);  
    vector<vector<int>> photosEnc = readEncoded(inputEncFile,datasize);
    //vector<vector<int>> photosEnc = photos;
    HashTable *ht[L];
    for (int i=0; i < L ; i++){
        ht[i] = new HashTable(K_DIM,photosEnc.size()/128,photosEnc[0].size());
    }

    for (int i=0;i<photosEnc.size();i++){
        for (int j=0; j<L ;j++ ){
            ht[j]->place(photosEnc[i],i);
        }
    }

    cout << "finished training\n";
    fflush(stdout);
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
    unordered_set<int> indexes;//set for not storing same items multiple times
    double af=0,maf=0;
    cout << "START OF SEARCH \n\n";
    std::chrono::microseconds avgReal(0),avgApprox(0); 
    for (int k=0;k<queriesEnc.size();k++){
        priority_queue<PQObject> pq;
        priority_queue<PQObject> pq_real;
        vector<int> queryEnc=queriesEnc[k];
        vector<int> query=queries[k];
        cout << "Query: " << k <<endl;
        indexes.clear();
        auto startLsh = chrono::high_resolution_clock::now();
        for(int i=0;i<L;i++){
            for (HTObject obj : ht[i]->getBucket(queryEnc)){ //for each object in bucket
                if (indexes.find(obj.index)==indexes.end()) {
                    PQObject pqo(dist(queryEnc,obj.p),obj.p,obj.index);
                    pq.push(pqo);
                    indexes.insert(obj.index);
                }
            }
        }
        auto endLsh = chrono::high_resolution_clock::now();
        auto durationLsh = chrono::duration_cast<std::chrono::microseconds>(endLsh - startLsh);

        auto startTrue = chrono::high_resolution_clock::now();
        for (int i=0;i<photos.size();i++){
            vector<int> obj=photos[i];
            PQObject pqo(dist(query,obj),obj,i);
            pq_real.push(pqo);
        }
        auto endTrue = chrono::high_resolution_clock::now();
        auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);

        //print data
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

    for (int i=0; i < L ; i++) {
        delete(ht[i]);
    }

    if (output.is_open()) output.close();

    return 0;
}