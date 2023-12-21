#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <unordered_set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include "helper.h"
#include "hashtable.h"
#include "hypercube.h"

using namespace std;

map<string, int> getClusterConf(string filename){//parse config file
    ifstream file(filename);
    string line;
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        exit(-1);
    }

    map<string,int> clusterConfMap;
    while (getline(file,line)){
        istringstream iss(line); //create string stream
        string key;
        int value;

        if (!(iss >> key >> value)) { //this line parses the line of the file with 2 tokens (1 string 1 int)
            cerr << "Failed to parse line: " << line << endl;
            exit(-1);
        }

        if (key == "number_of_clusters:") {
            clusterConfMap["K"]=value;
        } else if (key == "number_of_vector_hash_tables:") {
            clusterConfMap["L"]=value;
        } else if (key == "number_of_vector_hash_functions:") {
            clusterConfMap["K_LSH"]=value;
        } else if (key == "max_number_M_hypercube:") {
            clusterConfMap["M"]=value;
        } else if (key == "number_of_hypercube_dimensions:") {
            clusterConfMap["K_HCUBE"]=value;
        } else if (key == "number_of_probes:") {
            clusterConfMap["PROBES"]=value;
        }
    }
    return clusterConfMap;
}

void printCLuster(const vector<vector<double>> &centroids , vector<unordered_set<int>> &clusterAssignment , bool complete){
    streamsize c = cout.precision();
    cout.precision(3); //avoid long numbers
    for (int i=0; i < centroids.size() ; i++){
        cout << "CLUSTER-" << i+1 ;
        cout << ": size :" << clusterAssignment[i].size() << endl;
        cout << "centroid : " << endl;
        //print centroid
        for (int k=0 ; k<centroids[i].size() ; k++){
            cout << centroids[i][k] << " ";
            if ((k+1)%28==0) cout << endl;
        }
        if (complete) { //print image indexes in cluster
            cout << "Image numbers :\n{";
            int k=1;
            for (int index:clusterAssignment[i]){
                cout << index ;
                if (k==clusterAssignment[i].size()) cout << " }" << endl;
                else cout << " ,";
                k++;
            }
        }
        cout << endl;
    }
    cout.precision(c);
}

vector<vector<int>> init(vector<vector<int>> photos,int k){
    // initialize first centroid at random
    vector<vector<int>> centroids;
    int myRandom= rand() % photos.size();
    vector<int> firstCentroid=photos[myRandom];
    centroids.push_back(firstCentroid);
    //remove centroid from photo set (photo set is not passed by reference so it resets when we return)
    photos.erase(photos.begin()+myRandom);
    vector<double> d(photos.size()); //for saving d(i)
    int c=1; //count centroids
    while(c < k){
        //find dist to nearest centroid for each non-centroid (d(i))
        for (int i=0; i < photos.size(); i++ ){
            if (c==1){ //first loop
                //min distance to centroid is equal to distance to unique centroid
                d[i]=dist(photos[i],centroids[c-1]); 
            } else { //every other loop
                //min distance to centroid is equal to prev min dist , or dist to new centroid
                d[i]=min(d[i],dist(photos[i],centroids[c-1]));
            }
        }
        double m = *max_element(d.begin(), d.end()); //get dMax for normalisation of ps
        vector<double> ps;
        // calculate p(i)
        for(int i=0; i<photos.size(); i++ ){
            double sum=pow((d[i]/m),2); //d(i)^2
            if (i!=0) sum += ps[i-1]; //p(r)=p(r-1)+d(i)^2
            ps.push_back(sum);
        }
        double x = double(rand()) / RAND_MAX * ps[ps.size()-1]; //get a random number in (0,totalsums(n-t))
        int index = bSearch(ps, 0, ps.size()-1, x); //get corresponding index with binary search (p vector is already sorted since p(r)>=p(r-1))
        centroids.push_back(photos[index]); //add chosen centroid
        c++;
        if (c == k) break;
        //remove data from vectors for the element selected (to exclude it from centroid searching)
        photos.erase(photos.begin()+index);
        d.erase(d.begin()+index);
    }
    return centroids;
}

void removePointFromCentroid(const vector<int> &photo,vector<double> &centroid,unordered_set<int> &clusterSet,const int index){
    //remove the point from the cluster and subtract its value to the centroid (with percentage corresponding to cluster size)
    //new center = oldcenter * len / ( len - 1 ) - removing point / ( len - 1 )
    if (clusterSet.size()==1) { //if cluster becomes empty
        //this will never happen since when a clusteer is only one point 
        //there is no way that it needs to get out of the cluster, because it cant be nearer to another centroid
        for (int i=0;i<photo.size();i++){
            centroid[i]=0;
        }
        clusterSet.erase(index);
    } else {
        for (int i=0;i<photo.size();i++){
            centroid[i]*=clusterSet.size();
        }
        clusterSet.erase(index);
        for (int i=0;i<photo.size();i++){
            centroid[i]-=photo[i];
            //avoid negative values in centers due to computing arithmetic errors
            if (centroid[i]<0) centroid[i]=0; 
            else centroid[i]/=clusterSet.size();
        }
    }
}

void addPointToCentroid(const vector<int> &photo,vector<double> &centroid,unordered_set<int> &clusterSet,const int index){
    //add the point to the cluster and add its value to the centroid (with percentage corresponding to cluster size)
    //new center = old center * len / (len + 1) + new point / (len + 1)
    for (int i=0;i<photo.size();i++){
        centroid[i]*=clusterSet.size();
    }
    clusterSet.insert(index);
    for (int i=0;i<photo.size();i++){
        centroid[i]+=photo[i];
        centroid[i]/=clusterSet.size();
    }
}

void lloyds(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo , vector<unordered_set<int>> &clusterAssignment,bool complete){
    cout << "Algorithm : Lloyds\n";
    int maxIters = 1000;
    int iters=0;
    int changes=INT32_MAX;
    auto startCluster = chrono::high_resolution_clock::now();
    while(iters < maxIters && changes>0){
        changes=0; //reset changes
        for (int i=0;i<photos.size();i++){//for each photo
            //find nearest centroid
            double mindist=dist(photos[i],centroids[0]);
            int cluster=0;
            for (int j=1; j<centroids.size() ; j++){ 
                double d = dist(photos[i],centroids[j]);
                if (d < mindist) {
                    mindist=d;
                    cluster=j;
                }
            }

            //keep both directions of relation (for faster "belongs to" search)
            if (belongsTo[i]!=cluster){ //update centroids and relations tables
                if (belongsTo[i]!=-1) { //remove from old cluster
                    removePointFromCentroid(photos[i],centroids[belongsTo[i]],clusterAssignment[belongsTo[i]],i);
                }
                addPointToCentroid(photos[i],centroids[cluster],clusterAssignment[cluster],i); //add to new cluster
                belongsTo[i]=cluster;
                changes++; //note change
            }
        }
        iters++;
    }
    auto endCluster = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(endCluster - startCluster);
    printCLuster(centroids,clusterAssignment,complete);
    cout << "clustering time: " << double(duration.count()/1e6) << " seconds" << endl;
}

void lshRange(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo,
                vector<unordered_set<int>> &clusterAssignment,const int K_LSH,const int L,bool complete){
    cout << "Algorithm : Range Search LSH\n";
    auto startCluster = chrono::high_resolution_clock::now();
    //find the min and max dist between 2 centroids (used later)
    double minDist = -1.0;
    double maxDist = -1.0;
    for (int i=0 ; i<centroids.size()-1 ; i++){
        for (int j=i+1 ; j<centroids.size() ; j++){
            double d = dist(centroids[i],centroids[j]);
            if ( minDist<0 || d<minDist ) minDist=d;
            if ( maxDist<0 || d>maxDist ) maxDist=d;
        }
    }
    
    double range = minDist/2;
    //init hts
    HashTable *ht[L];
    for (int i=0; i < L ; i++){
        ht[i] = new HashTable(K_LSH,photos.size()/128,photos[0].size());
    }
    //populate hash tables
    for (int i=0; i<photos.size() ; i++){
        for (int j=0; j<L ;j++ ){
            ht[j]->place(photos[i],i);
        }
    }
    vector<double> dCl(photos.size(),-1.0); //init distances
    unordered_set<int> checked; //used to avoid checking same objects multiple times for each centroid
    unordered_set<int> assigned; //used to store the assigned points to avoid rechecking them
    
    int changes = INT32_MAX;
    int iters=1;
    while (iters < 3 || changes > 0){ //at least 3 iterations 
        changes=0; //reset changes
        //range search
        for (int i=0 ; i<centroids.size() ; i++){ //for each cluster
            checked.clear(); //reset checked for each cluster
            for (int j=0; j < L ; j++){ //for each hash func
                for (HTObject obj : ht[j]->getBucket(centroids[i])){ //for each object in it's bucket
                    if (assigned.count(obj.index)!=0) continue;//if already assigned ,skip
                    if (checked.count(obj.index)!=0) continue; //if already checked for this point ,skip
                    checked.insert(obj.index);
                    double d = dist(obj.p,centroids[i]);
                    if (d > range) continue; //check range
                    if (d < dCl[obj.index] || dCl[obj.index]==-1.0){ //check prev distance if it belongs to other cluster (before assignment)
                        belongsTo[obj.index] = i; //store nearest centroid for point so far
                        dCl[obj.index]=d; //store min dist for point so far
                    }
                }
            }
        }
        //assignment and centroid update
        for (int i=0; i < belongsTo.size() ; i++ ){
            //skip point if already assigned or not marked for assignment
            if (assigned.count(i)!=0 || belongsTo[i]<0) continue;
            addPointToCentroid(photos[i],centroids[belongsTo[i]],clusterAssignment[belongsTo[i]],i); //assign point
            assigned.insert(i); //mark point as assigned
            changes++; //note change
        }
        iters++;
        range*=2;
        if (range > maxDist) break; //avoid range becoming too big
    }
    //assign the points that dont have a class
    for (int i=0;i<belongsTo.size();i++){
        if(assigned.count(i)==0){//if its not assigned
            double minDist=dist(photos[i],centroids[0]);
            belongsTo[i]=0;
            for(int j=1;j<centroids.size();j++){
                double d = dist(photos[i],centroids[j]);
                if ( d < minDist ){
                    minDist = d;
                    belongsTo[i]=j;
                }
            }
            clusterAssignment[belongsTo[i]].insert(i);
        }
    }
    auto endCluster = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(endCluster - startCluster);
    printCLuster(centroids,clusterAssignment,complete);
    cout << "clustering time: " << double(duration.count()/1e6) << " seconds" << endl;
    //delete hash tables
    for (int i=0; i < L ; i++) {
        delete(ht[i]);
    }
}

void cubeRange(const vector<vector<int>> &photos , vector<vector<double>> &centroids , vector<int> &belongsTo,
                vector<unordered_set<int>> &clusterAssignment,const int K_HCUBE,const int M,const int PROBES,bool complete){
    cout << "Algorithm : Range Search HyperCube\n";
    auto startCluster = chrono::high_resolution_clock::now();
    //find the min and max dist between 2 centroids (used later)
    double minDist = -1.0;
    double maxDist = -1.0;
    for (int i=0 ; i<centroids.size()-1 ; i++){
        for (int j=i+1 ; j<centroids.size() ; j++){
            double d = dist(centroids[i],centroids[j]);
            if ( minDist<0 || d<minDist ) minDist=d;
            if ( maxDist<0 || d>maxDist ) maxDist=d;
        }
    }
    
    double range = minDist/2;
    //init and populate hypercube
    HyperCube hc(K_HCUBE,photos[0].size());
    for (int i=0;i<photos.size();i++){
        hc.place(photos[i],i);
    }
    vector<double> dCl(photos.size(),-1.0); //init distances
    unordered_set<int> assigned;//store the assigned points to avoid rechecking them
    
    int changes = INT32_MAX;
    int iters=1;
    bool last=false;
    while (iters < 3 || changes > 0){ //at least 3 iterations
        changes=0;
        for (int i=0 ; i<centroids.size() ; i++){ //for each cluster
            for (HCObject obj : hc.getBucket(centroids[i],PROBES,M)){ //for each object in it's bucket
                if (assigned.count(obj.index)!=0) continue;//if already assigned ,skip
                double d = dist(obj.p,centroids[i]);
                if (d > range) continue; //check range
                if (d < dCl[obj.index] || dCl[obj.index]==-1.0){ //check if it belongs to other cluster
                    belongsTo[obj.index] = i;
                    dCl[obj.index]=d;
                }
            }
        }
        for (int i=0; i < belongsTo.size() ; i++ ){
            if (assigned.count(i)!=0 || belongsTo[i]<0) continue;//if already assigned or not marked for assignment ,skip
            addPointToCentroid(photos[i],centroids[belongsTo[i]],clusterAssignment[belongsTo[i]],i); //assign point
            assigned.insert(i); //mark as assigned
            changes++; //note change
        }
        iters++;
        range*=2;
        if (range > maxDist) break; //avoid range getting too big
    }
    //assign the points and fill in those that dont have a class
    for (int i=0;i<belongsTo.size();i++){
        if(belongsTo[i]<0){//if its not assigned
            double minDist=dist(photos[i],centroids[0]);
            belongsTo[i]=0;
            for(int j=1;j<centroids.size();j++){
                double d = dist(photos[i],centroids[j]);
                if ( d < minDist ){
                    minDist = d;
                    belongsTo[i]=j;
                }
            }
            clusterAssignment[belongsTo[i]].insert(i);
        }
    }
    auto endCluster = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(endCluster - startCluster);
    printCLuster(centroids,clusterAssignment,complete);
    cout << "clustering time: " << double(duration.count()/1e6) << " seconds" << endl;
}

vector<double> silhouette(const vector<vector<int>> &photos, const vector<vector<double>> &centroids , const vector<unordered_set<int>> &clusterAssignment,const vector<int> &belongsTo){
    auto start = chrono::high_resolution_clock::now();
    double sil=0.0; //store total
    vector<double> s(centroids.size(),0.0);
    for (int i=0;i<photos.size();i++){
        //find second nearest
        double mindist = -1.0;
        int secNearest ;
        for(int j=0;j<centroids.size();j++){
            if (belongsTo[i]==j) continue;
            double d = dist(photos[i],centroids[j]);
            if (mindist==-1.0 || mindist > d ){
                mindist = d;
                secNearest = j;
            }
        }
        //compute avgA
        double avgA = 0.0;
        for(int index : clusterAssignment[belongsTo[i]]){
            if (index==i) continue;
            avgA += dist(photos[i],photos[index]);
        }
        avgA/=(clusterAssignment[belongsTo[i]].size());

        //compute avgB
        double avgB = 0.0;
        for(int index : clusterAssignment[secNearest]){
            avgB+=dist(photos[i],photos[index]);
        }
        avgB/=clusterAssignment[secNearest].size();

        double b = (avgA < avgB) ? 1 - avgA/avgB : avgB/avgA - 1;
        s[belongsTo[i]] += b; //add to cluster silhouette
        sil += b; //add to total silhouette
    }

    //calculating means and printing
    cout << "sillouete [" ;
    for (int i=0; i<s.size() ; i++) {
        s[i]/=clusterAssignment[i].size();
        cout << s[i] << ", ";
    }
    s.push_back(sil/photos.size());
    cout << "total : " << s[s.size()-1] << "]" << endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << "Compution time: " << double(duration.count()/1e6) << " seconds" << endl;
    return s;
}

vector<vector<double>> turnCentroidToNearest(vector<vector<double>> &centroids , vector<vector<int>> &pointsEnc ,  vector<vector<int>> &points){
    vector<vector<double>> retCentr(centroids.size(),vector<double>(points[0].size()));
    for (int k=0; k<centroids.size(); k++){
        //find nearest point
        double min=-1;
        int minInd = -1;
        for (int i=0 ; i<pointsEnc.size() ; i++){
            double d = dist(pointsEnc[i],centroids[k]);
            if (min == -1 || min>d){
                min = d;
                minInd = i;
            }
        }
        //assign in to centroid
        for (int i=0 ; i<retCentr[k].size() ; i++){
            retCentr[k][i] = (double) points[minInd][i];
        }
    }
    return retCentr;
}