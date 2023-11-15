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
#define R_DEF 10000
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
    const int R = (args.count("R")==1) ? stoi(args["R"]) : R_DEF;
    const bool skipId = (args.count("id")==0);
    string outputFile = (args.count("o")==1) ? args["o"] : "output.txt";
    string inputFile = (args.count("d")==1) ? args["d"] : "input.dat";
    string queryFile = (args.count("q")==1) ? args["q"] : "query.dat";

    if (args.count("d")==0){
        fprintf(stdout,"Please provide name of dataset file (Give \"def\" to continue with \"input.dat\")\n");
        cin >> inputFile;
        if (inputFile=="def") inputFile = "input.dat";
    }

    vector<vector<int>> photos=readInput(inputFile,10000);  
    HashTable *ht[L];
    for (int i=0; i < L ; i++){
        ht[i] = new HashTable(K_DIM,photos.size()/128);
    }

    for (int i=0;i<photos.size();i++){
        for (int j=0; j<L ;j++ ){
            ht[j]->place(photos[i],i);
        }
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
    unordered_set<int> indexes;//set for not storing same items multiple times
    
    while(queries.size()!=0){
        cout << "START OF SEARCH \n\n";
        for (int k=0;k<queries.size();k++){
            priority_queue<PQObject> pq;
            priority_queue<PQObject> pq_real;
            vector<int> query=queries[k];
            cout << "Query: " << k <<endl;
            indexes.clear();
            auto startLsh = chrono::high_resolution_clock::now();
            for(int i=0;i<L;i++){
                uint id = ht[i]->id(query);
                for (HTObject obj : ht[i]->getBucket(query)){ //for each object in bucket
                    if (indexes.find(obj.index)==indexes.end() && (skipId || obj.id==id)) {//only check for id if -id is given
                        PQObject pqo(dist(query,obj.p),obj.p,obj.index);
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
                cout << "Nearest Neighbour " << i+1 << " : " <<  pqo.getIndex() << endl;
                cout << "Distance :" << pqo.getDistance() << endl;
                cout << "Real Distance :" << pqo_real.getDistance() << endl;
                pq.pop();
                pq_real.pop();
                i++;
            }
            cout << "tLsh: " << double(durationLsh.count()/1e6) << " seconds" << endl;
            cout << "tReal: " << double(durationTrue.count()/1e6) << " seconds" << endl;

            cout << "Range Search :\n";
            int l=0;
            for(int i=0;i<L;i++){
                for (HTObject obj:ht[i]->getBucket(query)){ // for each object in bucket
                    double d = dist(obj.p,query);
                    if (d < R) { //check range
                        cout << "Index " << obj.index << " distance " << d << endl;
                        l++;
                    }
                    if (l>=20) break;
                }
                if (l>=20) break;
            }
            cout << endl;
            fflush(stdout);
        }
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
        queries = readQuery(queryFile,loops);  //read as many queries as given from user
    }

    for (int i=0; i < L ; i++) {
        delete(ht[i]);
    }

    if (output.is_open()) output.close();

    return 0;
}