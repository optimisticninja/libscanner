#pragma once

#include <string>
#include <vector>

using namespace std;

class Scanner
{
private:
    string target;
public:
    Scanner(string target)
        : target(target)
    {}
    
    vector<bool> quick_scan();
    vector<bool> range_scan(int lower, int higher);
    vector<bool> high_interest_scan();
};
