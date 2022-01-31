#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace std;

using freq_map_t = unordered_map<uint64_t, uint64_t>;
using ngram_map_t = unordered_map<uint64_t, vector<string>>;

class nGramCounter
{
public:
    ngram_map_t ngrams_collection;
    fs::path dir;
    int num_threads;
    int length_ngram;
    nGramCounter(const string &dir, int num_threads, int length_ngram);
    void compute();
    void display(freq_map_t &freq, int j);
    void process_file(fs::path &file, ngram_map_t &local_ngram, freq_map_t &local_freq);
};
