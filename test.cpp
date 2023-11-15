#include <iostream>
#include <vector>
#include <bitset>
#include <string.h>
#define L 3
using namespace std;

vector<int> getNprobes(int maxDif, string number,vector<int> vectorSoFar , int bitSoFar){
    if(maxDif==0) return vectorSoFar;
    if(bitSoFar==L) return vectorSoFar;
    string modified(number);
    modified[bitSoFar] = '0'+1-(modified[bitSoFar]-'0');
    if(maxDif==1) vectorSoFar.push_back(stoi(modified,nullptr,2));
    vectorSoFar=getNprobes(maxDif-1,modified,vectorSoFar,bitSoFar+1);
    vectorSoFar=getNprobes(maxDif,number,vectorSoFar,bitSoFar+1);
    return vectorSoFar;
}


int main(){
    bitset<32> binary(7);
    cout << binary << endl;
    string num = binary.to_string().substr(32-3,32);
    cout << num <<endl;
    vector<int> nums ;
    nums = getNprobes(1,num,nums,0);
    nums = getNprobes(2,num,nums,0);
    for (int n : nums) {
        cout << n << endl; 
    }
    return 0; 
}