#include "nGram_counter.hpp"
#include "fileUtil.hpp"

#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <functional>
#include <future>

using namespace std;

using promise_t = vector<promise<unordered_map<uint64_t, uint64_t>>>;
using future_t = vector<future<unordered_map<uint64_t, uint64_t>>>;
using pair_t = pair<uint64_t, uint64_t>;
using freq_map_t = unordered_map<uint64_t, uint64_t>;
using ngram_map_t = unordered_map<uint64_t, vector<string>>;

nGramCounter::nGramCounter(const string &dir, int num_threads, int length_ngram) : dir(dir), num_threads(num_threads), length_ngram(length_ngram) {}

void nGramCounter::compute()
{

    mutex wc_mtx;

    vector<fs::path> files_lst = get_files_lst(dir, [](const string &extension)
                                               { return extension == ".txt"; });

    vector<int> file_index;
    for (int i = 0; i < num_threads; ++i)
        file_index.push_back(i);

    vector<promise_t> promises;
    vector<future_t> futures;
    for (int i = 0; i < num_threads; ++i)
    {
        promises.emplace_back();
        futures.emplace_back();
        for (int j = 0; j < num_threads; ++j)
        {
            futures[i].emplace_back();
            promises[i].emplace_back();
            futures[i][j] = promises[i][j].get_future();
        }
    }

    auto sweep = [this, &promises, &futures, &files_lst, &file_index, &wc_mtx](int j, int num_threads)
    {
        freq_map_t local_freq;
        ngram_map_t local_ngram;
        while (file_index[j] < files_lst.size())
        {
            process_file(files_lst[file_index[j]], local_ngram, local_freq);
            file_index[j] += num_threads;
        }

        vector<freq_map_t> assign(num_threads);
        for (auto [key, value] : local_freq)
            assign[key % num_threads].insert({key, value});

        lock_guard<mutex> lock(wc_mtx);
        for (auto [key, value] : local_freq)
            if (ngrams_collection.find(key) == ngrams_collection.end())
                ngrams_collection.insert({key, local_ngram[key]});
        for (int i = 0; i < num_threads; i++)
            promises[i][j].set_value(assign[i]);
    };

    auto reduce = [this, &promises, &futures, &files_lst, &file_index, &wc_mtx](int j)
    {
        vector<freq_map_t> local_freqs;
        for (int i = 0; i < futures[j].size(); i++)
            local_freqs.push_back(futures[j][i].get());

        freq_map_t freq;
        for (auto &local_freq : local_freqs)
        {
            for (auto [key, value] : local_freq)
            {
                if (freq.find(key) == freq.end())
                    freq.insert({key, value});
                else
                    freq.find(key)->second += value;
            }
        }
        lock_guard<mutex> lock(wc_mtx);
        display(freq, j);
    };

    vector<thread> reducers;
    vector<thread> workers;

    for (int i = 0; i < num_threads; i++)
        reducers.push_back(thread(reduce, i));
    for (int i = 0; i < num_threads; i++)
        workers.push_back(thread(sweep, i, num_threads));
    for (auto &worker : workers)
        worker.join();
    for (auto &reducer : reducers)
        reducer.join();
}

void nGramCounter::display(freq_map_t &freq, int j)
{
    vector<pair_t> freq_vec(freq.size());
    int index = 0;
    for (auto [key, cnt] : freq)
        freq_vec[index++] = {key, cnt};

    sort(freq_vec.begin(), freq_vec.end(), [](const pair_t &p1, const pair_t &p2)
         { return p1.second > p2.second || (p1.second == p2.second && p1.first < p2.first); });

    cout << " " << endl;
    cout << "Thread " << j + 1 << endl;
    cout << " " << endl;

    int num_displayed = 1;
    for (auto [key, value] : freq_vec)
    {
        if (num_displayed <= 10)
        {
            auto words = ngrams_collection[key];
            string ngram = "";
            int i = 0;
            while (i < words.size())
            {
                ngram += words[i];
                if (i == words.size() - 1)
                {
                }
                else
                    ngram += " ";
                i++;
            }
            cout << ngram << " -> " << value << endl;
            num_displayed += 1;
        }
        else
            break;
    }
}

string transform(string line)
{
    transform(line.begin(), line.end(), line.begin(), [](unsigned char c)
              { return tolower(c); });
    line = regex_replace(line, regex("\\s+"), " ");
    line = regex_replace(line, regex("[^a-zA-Z\\s\\:]"), " | ");
    return line;
}

void nGramCounter::process_file(fs::path &file, ngram_map_t &local_ngram, freq_map_t &local_freq)
{
    vector<string> words;
    string line;
    ifstream fin(file);
    hash<string> hashed;

    while (getline(fin, line))
    {
        string word;
        line = transform(line);
        stringstream buffer(line);
        while (getline(buffer, word, ' '))
        {
            if (words.size() == length_ngram)
            {
                string ngram = "";
                for (auto &word : words)
                    ngram += word;
                uint64_t key = hashed(ngram);
                if (local_freq.find(key) == local_freq.end())
                    local_freq.insert({key, 1});
                else
                    local_freq.find(key)->second += 1;
                if (local_ngram.find(key) == local_ngram.end())
                    local_ngram.insert({key, words});
                words.erase(words.begin());
            }
            if (word == "|")
                words.clear();
            else if (word != "")
                words.push_back(word);
        }
    }
    fin.close();
}
