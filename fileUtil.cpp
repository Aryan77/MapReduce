#include "fileUtil.hpp"
#include <list>

using namespace std;
using namespace fs;

vector<path> get_files_lst(const path &dir, function<bool(const string &)> pred)
{
    list<path> files_lst;
    for (auto &entry : recursive_directory_iterator(dir))
    {
        if (entry.is_regular_file())
        {
            path cur_file = entry.path();
            string type(cur_file.extension());
            if (pred(type))
                files_lst.push_back(move(cur_file));
        }
    }
    return vector<path>(make_move_iterator(files_lst.begin()), make_move_iterator(files_lst.end()));
}
