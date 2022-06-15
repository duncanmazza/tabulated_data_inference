/**
 * Example script showing usage of the `classifyColumns` function on data files specified by hard-coded paths.
 *
 * @author Duncan Mazza
 */

#include "tabulated_data_inference.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


int getFileLines(const string &target, vector<string> &ret) {
    string line;
    ifstream targetFile(target);
    if (targetFile.is_open()) {
        while (getline(targetFile, line)) {
            ret.push_back(line);
        }
        targetFile.close();
    } else {
        cerr << "Could not open file " << target << " (skipping)" << endl;
        return 0;
    }
    return 1;
}


int main() {
    vector<string> fileTargets{  // Populate this vector with the files to parse
            R"(tests/test_targets/xf-naca2408-il-50000.csv)",
            R"(tests/test_targets/acsm_shortened.csv)"
    };

    vector<vector<string>> filesLines;
    size_t fileIdx = -1;
    for (const auto &target: fileTargets) {
        fileIdx++;
        vector<string> fileLines;
        if (!getFileLines(target, fileLines)) { continue; }
        filesLines.push_back(fileLines);
    }

    fileIdx = -1;
    auto parser = MpcParserTWrapper();
    for (const auto &thisFileLines: filesLines) {
        fileIdx++;
        cout << "Parsing " << fileTargets.at(fileIdx) << endl;

        cout << "Finding delimiter..." << endl;
        auto delimRet = getDelim(thisFileLines);

        cout << "Finding fields..." << endl;
        vector<vector<string>> fieldRet;
        int consistentFields = getFields(thisFileLines, get<0>(delimRet), fieldRet, get<1>(delimRet));
        if (!consistentFields) {
            cout << "Could not find a consistent number of fields in " << fileTargets.at(fileIdx) << endl;
        }

        cout << "Classifying fields..." << endl;
        vector<tuple<string, FieldCls>> classificationRet;
        classifyColumns(fieldRet, classificationRet, parser);

        cout << "Data insights for " << fileTargets.at(fileIdx) << ":" << endl;
        for (auto classification: classificationRet) {
            cout << " - " << get<0>(classification) << " (" << FieldClsCorrespondingNames[get<1>(classification)] << ")"
                 << endl;
        }
        cout << endl;
    }
    return 0;
}