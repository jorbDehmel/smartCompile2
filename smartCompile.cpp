#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

// Recursive ls without folder names (except "./"):
// find -print >output.txt

void makeObjs(vector<string> &includes, string &stem) {

}

vector<string> makeBin(vector<string> &objs, string &stem) {

}

int main(char *argv[], int argc) {
    vector<string> includes;

    // find all .cpp
    // for all without main:
        // make objs
    // for all with main:
        // make obj
        // build bin w/ all .o that don't have main

    return 0;
}
