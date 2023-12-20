#include "hashtable.h"
#include <cstdlib>

//constructor init parameters
HashTable::HashTable(int K_DIM,int TABLESIZE,int DIM) :TABLESIZE(TABLESIZE),K_DIM(K_DIM),v_vects(K_DIM, vector<double>(DIM)),rs(K_DIM),ts(K_DIM),table(TABLESIZE){
    //init distributions
    random_device rd;
    mt19937 generator(rd());
    normal_distribution<double> dist(0.0, 1.0);
    uniform_int_distribution<uint> r_dist(1,WIN);
    uniform_int_distribution<uint> t_dist(0,WIN*10);

    for (int i = 0 ; i < K_DIM ; i++){
        rs[i]=r_dist(generator); //generate r per h function
        ts[i]=double(t_dist(generator)/10.0); //generate t per h function 
        for (int j = 0 ; j < DIM ; j++){
            v_vects[i][j] = dist(generator); //create K vectors v (dimension d) for each h function as a K*D array
        }
    }
}

uint HashTable::g(vector<int> p){ //g computation for int vectors
    uint g=0;
    for (int i=0;i<K_DIM;i++){ //for K h functions
        double res=0.0;
        //find p*v
        for (int j=0 ; j<v_vects[i].size() ; j++){ 
            res+=(v_vects[i][j])*(p[j]);
        }
        //+t div w
        uint h=unsigned(floor((res+ts[i])/WIN)); //cast to unsigned to get only positive hash values
        g+=h*rs[i];
    }
    return g;
}

uint HashTable::g(vector<double> p){ //g computation for double vectors
    uint g=0;
    for (int i=0;i<K_DIM;i++){ //for K h functions
        double res=0.0;
        //find p*v
        for (int j=0 ; j<v_vects[i].size() ; j++){ 
            res+=(v_vects[i][j])*(p[j]);
        }
        //+t div w
        uint h=unsigned(floor((res+ts[i])/WIN));
        g+=h*rs[i];
    }
    return g;
}

void HashTable::place(vector<int> p,int index){
    uint id=this->g(p) % M_NUM; //store id
    vector<int> p_place = p;
    HTObject ht(id,p_place,index);
    this->table[id % TABLESIZE].push_back(ht);
}

void HashTable::print(){
    for (int i=0 ; i < TABLESIZE ; i++){
        cout << "bucket " << i << ": (" << table[i].size() << "points) ";
        // for (int j=0 ; j < table[i].size() ; j++){
        //     table[i][j].print();
        // }
        cout << "\n";
    }
}