#!/bin/bash

# g++ version must be >=8 
g++ -O3 -std=c++17 map_reduce.cpp nGram_counter.cpp fileUtil.cpp -lpthread -lstdc++fs -o countNgrams
