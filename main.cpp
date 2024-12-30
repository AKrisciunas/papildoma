#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <string>
#include <iomanip> 
#include <cctype> // tolower, isalnum

using namespace std;

// remove punctuation
string cleanWord(const string &word) {
    string cleaned;
    for (unsigned char c : word) { 
        if (isalnum(c)) cleaned += tolower(c);
    }
    return cleaned;
}

int main() {
    ifstream inputFile("input.txt");
    ofstream wordOutput("word_count.txt");
    ofstream urlOutput("urls.txt");

    if (!inputFile.is_open() || !wordOutput.is_open() || !urlOutput.is_open()) {
        cerr << "Error: Unable to open one of the files." << endl;
        return 1;
    }

    map<string, int> wordCount;
    map<string, set<int>> wordLines;
    set<string> urls;

    string fileLine;
    int lineNumber = 0;

    regex urlRegex(R"((http|https):\/\/\S+|www\.\S+|\b\w+\.\w{2,}\b)");
    
    while (getline(inputFile, fileLine)) {
        lineNumber++;

        istringstream lineStream(fileLine);
        string word;
        smatch urlMatch;

        string tempLine = fileLine;
        while (regex_search(tempLine, urlMatch, urlRegex)) {
            urls.insert(urlMatch.str());
            tempLine = urlMatch.suffix();
        }

        while (lineStream >> word) {
            string cleaned = cleanWord(word);
            if (!cleaned.empty()) {
                wordCount[cleaned]++;
                wordLines[cleaned].insert(lineNumber);
            }
        }
    }

    wordOutput << left << setw(20) << "Word" << "  " << "Count" << "  Lines" << endl;
    for (const auto &entry : wordCount) {
        if (entry.second > 1) {
            wordOutput << left << setw(20) << entry.first << "  " << setw(5) << entry.second << "  ";
            for (int lineNum : wordLines[entry.first]) {
                wordOutput << lineNum << " ";
            }
            wordOutput << endl;
        }
    }

    urlOutput << "URLs found:" << endl;
    for (const string &url : urls) {
        urlOutput << url << endl;
    }

    inputFile.close();
    wordOutput.close();
    urlOutput.close();

    return 0;
}
