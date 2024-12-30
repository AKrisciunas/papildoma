#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <string>
#include <iomanip>
#include <cctype> // tolower, isalnum
#include <codecvt>
#include <locale>

using namespace std;

std::wstring utf8ToWstring(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string wstringToUtf8(const std::wstring &wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// remove punctuation 
string cleanWord(const string &word) {
    std::wstring wword = utf8ToWstring(word);
    std::wstring wcleaned;
    for (wchar_t wc : wword) {
        if (iswalnum(wc)) { 
            wcleaned += towlower(wc); 
        }
    }
    return wstringToUtf8(wcleaned);
}

void findWordsWithSubstring(const string &substring, const set<string> &uniqueWords, const string &outputFilename) {
    ofstream output(outputFilename);
    if (!output.is_open()) {
        cerr << "Error: Unable to open the output file." << endl;
        return;
    }

    output << "Words containing \"" << substring << "\":" << endl;
    for (const string &word : uniqueWords) {
        if (word.find(substring) != string::npos) {
            output << word << endl;
        }
    }
    output.close();
}

int main() {
    ifstream inputFile("input.txt");
    ofstream wordOutput("word_count.txt");
    ofstream urlOutput("urls.txt");

    if (!inputFile.is_open() || !wordOutput.is_open() || !urlOutput.is_open()) {
        cerr << "Error: Unable to open one of the files." << endl;
        return 1;
    }

    map<string, int> wordCount;         // to tell how many
    map<string, set<int>> wordLines;    // to tell where word occurs
    set<string> urls;
    set<string> uniqueWords;           // unique words search

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
                uniqueWords.insert(cleaned); 
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

    cout << "Enter a substring to search for: ";
    string substring;
    cin >> substring;
    findWordsWithSubstring(substring, uniqueWords, "output2.txt");

    inputFile.close();
    wordOutput.close();
    urlOutput.close();

    return 0;
}
