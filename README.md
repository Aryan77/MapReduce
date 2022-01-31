# A MapReduce Implementation in C++

This project was an attempt to better understand Google's [MapReduce](https://static.googleusercontent.com/media/research.google.com/en//archive/mapreduce-osdi04.pdf) algorithm, by implementing it from scratch in `C++`. The code essentially takes as input the path to a directory full of text files, counts and sorts the Ngrams (contiguous sequence of `n` words - `n` is also a constant provided by the user) in those texts by frequency, and reports back the most popular Ngrams found by each of the worker threads. 

The obvious advantage of using this MapReduce pattern to accomplish this task is the enormous potential for scalability - significantly more files can be processed than could be accomplished in a single threaded program. 

The fact that the worker threads maintain their own local data structures and only update the global data structure once they have all the data provides another huge advantage: there is very little need for locking any large data structures - making code both significantly easier to write and less prone to errors and slow-downs. It is clear why MapReduce has been so succesfully used in big data processing.

To run the code locally, you will need at least version 8 of `gcc` on your machine.

 1. Clone this repository and navigate to the directory it is in.
 2. Run `./compile.sh`. If you get permission-related errors, run `chmod u+x compile.sh` first and then run `./compile.sh`. This should produce an executable `countNgrams`.
 3. Run this executable as: `./countNgrams <relative-path-to-directory-of-txt-files> -t=<number-of-worker-threads> -n=<size-of-ngram>.`
