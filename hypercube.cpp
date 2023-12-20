#include "hypercube.h"
#include <cstdlib>
#include <cmath>
#include <bitset>

//recursive function to retrieve all binary numbers that differ from a given binary number 'maxDif' diggits
//prolog logic :) 
vector<int> HyperCube::getNprobes(int maxDif, string number,vector<int> vectorSoFar , int bitSoFar){
    if(maxDif==0) { //if dif = starting maxDif convert number , add to vector and return
        vectorSoFar.push_back(stoi(number,nullptr,2));
        return vectorSoFar;
    }
    if(bitSoFar==D_DIM) return vectorSoFar; // bit_So_far out of range
    string modified(number); //copy binary string
    modified[bitSoFar] = '0'+('1'-modified[bitSoFar]); //modify its bitSoFar'th diggit 
    vectorSoFar=getNprobes(maxDif-1,modified,vectorSoFar,bitSoFar+1); //either modify the bit and go to next diggit
    vectorSoFar=getNprobes(maxDif,number,vectorSoFar,bitSoFar+1); //or dont and go to next diggit
    return vectorSoFar; //return results
}

HyperCube::HyperCube(int D_DIM,int DIM) :D_DIM(D_DIM),v_vects(D_DIM, vector<double>(DIM)),rs(D_DIM),ts(D_DIM),table(pow(2,D_DIM)),f(D_DIM){
    //init distributions
    random_device rd;
    mt19937 generator(rd());
    normal_distribution<double> dist(0.0, 1.0);
    uniform_int_distribution<uint> r_dist(1,HP_WIN*5);
    uniform_int_distribution<uint> t_dist(0,HP_WIN*10);

    for (int i = 0 ; i < D_DIM ; i++){
        rs[i]=r_dist(generator); //generate r per h function
        ts[i]=double(t_dist(generator)/10.0); //generate t per h function 
        for (int j = 0 ; j < DIM ; j++){
            v_vects[i][j] = dist(generator); //create K vectors v (dimension d) for each h function as a K*D array
        }
    }
}

int HyperCube::hash(vector<int> p){
    char val[D_DIM+1];
    for (int i=0;i<D_DIM;i++){ //for K h functions
        double res=0.0;
        //find p*v
        for (int j=0;j<v_vects[i].size();j++){ 
            res+=(v_vects[i][j])*(p[j]);
        }
        //+t div w
        uint h=unsigned(floor((res+ts[i])/HP_WIN));
        if (f[i].count(h)==0) f[i][h]=rand()%2; // if (fi(hi(p))) isn't saved , flip a coin and save the results
        val[i]='0'+f[i][h]; //0 or 1
    }
    val[D_DIM]='\0';
    int decimalValue = strtol(val, NULL, 2); //convert string to 
    return decimalValue;
}

int HyperCube::hash(vector<double> p){
    char val[D_DIM+1];
    for (int i=0;i<D_DIM;i++){ //for K h functions
        double res=0.0;
        //find p*v
        for (int j=0;j<v_vects[i].size();j++){ 
            res+=(v_vects[i][j])*(p[j]);
        }
        //+t div w
        uint h=unsigned(floor((res+ts[i])/HP_WIN));
        uint fVal;
        if (f[i].count(h)==0) f[i][h]=rand()%2; // if (fi(hi(p))) isn't saved , save it
        val[i]='0'+f[i][h];//0 or 1
    }
    val[D_DIM]='\0';
    int decimalValue = strtol(val, NULL, 2);
    return decimalValue;
}


void HyperCube::place(vector<int> p,int index){
    vector<int> p_place = p;
    HCObject ht(p_place,index);
    this->table[hash(p)].push_back(ht);
}

void HyperCube::print(){
    for (int i=0 ; i < table.size() ; i++){
        cout << "bucket " << i << ": (" << table[i].size() << "points) ";
        // for (int j=0 ; j < table[i].size() ; j++){
        //     table[i][j].print();
        // }
        cout << "\n";
    }
}

vector<HCObject> HyperCube::getBucketHelper(int h,int probes,int maxPoints){
    bitset<32> binary(h); //convert to binary (hope we dont get more than 36 h functions :) )
    string number = binary.to_string().substr(32-D_DIM,32);//cut last D_DIM digits (=dimension of f)
    vector<HCObject> dataVector; //for storing points
    int j=0; //points in return vector so far
    int i=0; //bit differences (start with 0)
    int pr=0; //number of probes
    while(i < D_DIM && pr <= probes && j<maxPoints){//just make sure we dont get out of number bits range
        vector<int> probesVector;
        probesVector=getNprobes(i,number,probesVector,0);//get probes with i bits different
        for (int n: probesVector){
            for (HCObject hco : table[n]){
                dataVector.push_back(hco);
                j++;
                if (j>=maxPoints) break;//end (enough points)
            }
            pr++;
            if (j>=maxPoints || pr>=probes) break; //end (enough points or probes)
        }
        i++;
        if (j>=maxPoints || pr>=probes) break; //end (enough points or probes)
    }
    return dataVector;
}