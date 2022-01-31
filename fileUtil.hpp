#include <filesystem>
#include <functional>
#include <vector>

namespace fs = std::filesystem;
using namespace fs;
using namespace std;

vector<path> get_files_lst(const path &dir, function<bool(const string &)> pred);
