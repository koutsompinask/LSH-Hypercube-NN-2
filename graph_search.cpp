#include <stdio.h>
#include <chrono>
#include <queue>
#include <fstream>
#include <algorithm>
#include "pqobject.h"
#include "gnns.h"
#include "mrng.h"
#include "helper.h"
#define K_DEF 50
#define E_DEF 30
#define R_DEF 1
#define N_DEF 1
#define L_DEF 20

int main(int argc,char *argv[]){
    //read args
    map<string,string> args=getArguments(argc,argv);
    const int E = (args.count("E")==1) ? stoi(args["E"]) : E_DEF;
    const int k = (args.count("k")==1) ? stoi(args["k"]) : K_DEF;
    const int N = (args.count("N")==1) ? stoi(args["N"]) : N_DEF;
    const int R = (args.count("R")==1) ? stoi(args["R"]) : R_DEF;
    const int l = (args.count("l")==1) ? stoi(args["l"]) : L_DEF;
    const int m = (args.count("m")==1) ? stoi(args["m"]) : 1;
    string outputFile = (args.count("o")==1) ? args["o"] : "output3.txt";
    string inputFile = (args.count("d")==1) ? args["d"] : "input.dat";
    string queryFile = (args.count("q")==1) ? args["q"] : "query.dat";
    if (args.count("d")==0){
        fprintf(stdout,"Please provide name of dataset file (Give \"d\" to continue with \"input.dat\")\n");
        cin >> inputFile;
        if (inputFile=="d") inputFile = "input.dat";
    }
    fprintf(stdout,"How many data do you want to include?\n");
    int x;
    cin >> x;
    vector<vector<int>> photos=readInput(inputFile,x);
    Graph *g; //abstract class
    string method;
    //init abstract class based on argument
    if (m==1){
        g = new Gnns(photos,5,4,k,E,R);
        method = "GNNS";
    } else {
        g = new Mrng(photos,l);
        method = "MRNG";
    }
    if (args.count("q")==0){
        fprintf(stdout,"Please provide name of query file (Give \"d\" to continue with \"query.dat\")\n");
        cin >> queryFile;
        if (queryFile=="d") queryFile = "query.dat";
    }
    int loops;
    fprintf(stdout,"how many queries do you want to execute ?\n");
    cin >> loops;
    vector<vector<int>> queries = readQuery("query.dat",loops);

    if (args.count("o")==0){
        fprintf(stdout,"Please provide name of output file (Give \"d\" to continue with \"output3.txt\")\n");
        cin >> outputFile;
        if (outputFile=="d") outputFile = "output3.txt";
    }

    ofstream output ;
    output=ofstream(outputFile);
    if (output.is_open()) {
        cout.rdbuf(output.rdbuf());
    }
    cout << method << " Results\n";
    string contFlag;
    bool repeat=true;
    while(repeat){ //loop in case user wants to re-execute some queries
        double maf=-1; //keep maf 
        std::chrono::microseconds avgReal(0),avgApprox(0); 
        int qCount=0; 
        for (auto q: queries){
            cout << "Query: " << qCount << endl;
            priority_queue<PQObject> S = g->search(q,avgApprox); //call search on graph
            priority_queue<PQObject> pq_real; //search real dist
            auto startTrue = chrono::high_resolution_clock::now();
            for (int i=0;i<photos.size();i++){
                vector<int> obj=photos[i];
                PQObject pqo(dist(q,obj),obj,i);
                pq_real.push(pqo);
            }
            auto endTrue = chrono::high_resolution_clock::now();
            auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);
            avgReal+=durationTrue;
            int i = 0;
            double af;
            while (!S.empty() && !pq_real.empty() && i< N ){
                PQObject pqo = S.top();
                cout << "Nearest Neighbour " << i+1 << " : " << pqo.getIndex() << endl;
                cout << "distanceApproximate: " << pqo.getDistance() << endl;
                PQObject pqo_real = pq_real.top();
                cout << "distanceTrue:" << pqo_real.getDistance() << endl;
                S.pop();
                pq_real.pop();
                af=pqo.getDistance()/pqo_real.getDistance();
                if (af>maf) maf=af;
                i++;
            }
            qCount++;
        }
        cout << "tAverageApproximate: " << double((avgApprox/queries.size()).count()/1e6) << endl;
        cout << "tAverageReal: " << double((avgReal/queries.size()).count()/1e6) << endl;
        cout << "MAF: " << maf << endl;
        fprintf(stdout,"Do you want to continue with another query ?(y/n)\n");
        cin >> contFlag;
        if (contFlag[0]!='y') break; //if not 'y' provided , stop the loop
        fprintf(stdout,"Please provide name of query file (Give \"d\" to continue with previous file\n");
        string prev = queryFile;
        cin >> queryFile;
        if (queryFile=="d") queryFile = prev;
        fprintf(stdout,"how many queries do you want to execute ?\n");
        cin >> loops;
        queries = readQuery(queryFile,loops);
    }
    delete(g);
    return 0;
}