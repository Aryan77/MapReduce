#include <iostream>
#include "nGram_counter.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int t, n;
    if (argc < 4)
    {
        cout << "Usage: " << argv[0] << " <dir> -t=<num-threads> -n=<length-ngrams>" << endl;
        return 1;
    }
    try
    {
        t = stoi(string(argv[2]).substr(3));
        n = stoi(string(argv[3]).substr(3));
    }
    catch (...)
    {
        cout << "Usage: " << argv[0] << " <dir> -t=<num-threads> -n=<length-ngrams>" << endl;
        return 1;
    }
    nGramCounter counter(argv[1], t, n);
    counter.compute();
    return 0;
}
