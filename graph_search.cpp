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
    string inputEncFile = (args.count("de")==1) ? args["de"] : "x_train_enc4.txt";
    string queryEncFile = (args.count("qe")==1) ? args["qe"] : "x_test_enc4.txt";
    fprintf(stdout,"How many data do you want to include?\n");
    int x;
    cin >> x;
    vector<vector<int>> photos = readInput(inputFile,x);
    vector<vector<int>> photosEnc = readEncoded(inputEncFile,x);
    //vector<vector<int>> photosEnc = photos;
    Graph *g; //abstract class
    string method;
    //init abstract class based on argument
    if (m==1){
        g = new Gnns(photosEnc,5,4,k,E,R);
        method = "GNNS";
    } else {
        g = new Mrng(photosEnc,l);
        method = "MRNG";
    }
    int loops;
    fprintf(stdout,"how many queries do you want to execute ?\n");
    cin >> loops;
    vector<vector<int>> queries = readQuery("query.dat",loops);
    vector<vector<int>> queriesEnc = readEncoded(queryEncFile,loops);
    //vector<vector<int>> queriesEnc = queries;

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
    double avgAf,maf=-1; //keep maf 
    std::chrono::microseconds avgReal(0),avgApprox(0); 
    int qCount=0; 
    for (int k=0; k<queriesEnc.size() ; k++){
        cout << "Query: " << qCount << endl;
        priority_queue<PQObject> S = g->search(queriesEnc[k],avgApprox); //call search on graph
        priority_queue<PQObject> pq_real; //search real dist
        auto startTrue = chrono::high_resolution_clock::now();
        for (int i=0;i<photos.size();i++){
            vector<int> obj=photos[i];
            PQObject pqo(dist(queries[k],obj),obj,i);
            pq_real.push(pqo);
        }
        auto endTrue = chrono::high_resolution_clock::now();
        auto durationTrue = chrono::duration_cast<std::chrono::microseconds>(endTrue - startTrue);
        avgReal+=durationTrue;
        int i = 0;
        double af;
        while (!S.empty() && !pq_real.empty() && i< N ){
            PQObject pqo = S.top();
            double d = dist(queries[k],photos[pqo.getIndex()]);
            cout << "Nearest Neighbour " << i+1 << " : " << pqo.getIndex() << endl;
            cout << "distanceApproximate: " << d << endl;
            PQObject pqo_real = pq_real.top();
            cout << "distanceTrue:" << pqo_real.getDistance() << endl;
            S.pop();
            pq_real.pop();
            af=d/pqo_real.getDistance();
            if (af>maf) maf=af;
            avgAf+=af/queriesEnc.size();
            i++;
        }
        qCount++;
    }
    cout << "tAverageApproximate: " << double((avgApprox/queries.size()).count()/1e6) << endl;
    cout << "tAverageReal: " << double((avgReal/queries.size()).count()/1e6) << endl;
    cout << "MAF: " << maf << endl;
    cout << "avgAF: " << avgAf << endl;

    delete(g);
    return 0;
}