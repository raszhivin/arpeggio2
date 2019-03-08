#ifndef BJORKLUND_H_INCLUDED
#define BJORKLUND_H_INCLUDED

#include <cmath>
#include <stdexcept>
#include <vector>
#include <algorithm>
using namespace std;

class Bjorklund{

public:

    Bjorklund(){};
    Bjorklund(int step, int pulse): lengthOfSeq(step), pulseAmt(pulse){
        if(lengthOfSeq <= 0 || lengthOfSeq < pulseAmt) lengthOfSeq = pulseAmt;
        init();
    };
    ~Bjorklund(){
        remainder.clear();
        count.clear();
        sequence.clear();
    };

    vector<int>remainder;
    vector<int>count;
    vector<bool>sequence; //accessing sequence directly is discouraged. use LoadSequence()

    int lengthOfSeq;
    int pulseAmt;

    void init();
    void init(int step, int pulse);
    void iter();
    void buildSeq(int slot);
    void clear();
    int getSequence(int index) {return sequence.at(index);};
    int size() {return (int)sequence.size();};

private:

};

#endif // BJORKLUND_H_INCLUDED
