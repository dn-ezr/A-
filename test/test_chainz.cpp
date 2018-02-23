#include <chainz.hpp>
#include <string>
#include <iostream>

using namespace std;

struct st {
    string name;
    chainz<st> operands;

    void out() {
        cout << name;
        if( operands.size() != 0 ) {
            cout << "[";
            for( auto& s : operands ) {
                s.out();
            }
            cout << "]";
        }
    }
};

int main( int argc, char **argv ) {
    chainz<st> instances;
    st temp;
    temp.name = "b";
    temp.operands << temp;
    temp.out();cout << endl;
    temp.name = "c";
    temp.operands[0].operands << temp;
    temp.out();cout << endl;
    temp.name = "d";
    temp.operands[0].operands << temp;
    temp.name = "a";
    temp.out();cout << endl;
}