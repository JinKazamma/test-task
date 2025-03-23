#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[1] << "'" <<endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file '" << argv[1] << "'" << endl;
        return 1;
    }

    unordered_map<string, int> word_counts;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string word;
        while (iss >> word) {
            ++word_counts[word];
        }
    }

    for (const auto& pair : word_counts) {
        cout << pair.first << " - " << pair.second << endl;
    }

    return 0;
}