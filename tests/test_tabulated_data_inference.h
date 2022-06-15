/**
 * Fixture definitions for delimited_file_inference.cpp interface.
 *
 * @author Duncan Mazza
 */

#ifndef TEST_TABULATED_DATA_INFERENCE_H
#define TEST_TABULATED_DATA_INFERENCE_H

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include <tabulated_data_inference.h>

/**
 * Base class used for other text fixtures that provides a utility function for loading a file's lines of text as a
 * vector of strings.
 */
class FileReaderHelperTestFixture : public testing::Test {
protected:
    vector<vector<string>> filesLines;

    void populateFilesLines(const vector<string> &fileTargets) {
        size_t targetIdx = -1;
        for (const auto &target: fileTargets) {
            targetIdx++;
            vector<string> fileLines;
            if (!FileReaderHelperTestFixture::getFileLines(target, fileLines)) { continue; }
            this->filesLines.push_back(fileLines);
        }
    }

public:
    static int getFileLines(const string &target, vector<string> &ret) {
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
};


class DelimTestFixture : public FileReaderHelperTestFixture {
protected:
    vector<string> fileTargets{
            R"(tests/test_targets/shortened_SEMS.dat)",
            R"(tests/test_targets/long_SEMS.dat)",
            R"(tests/test_targets/acsm_shortened.csv)",
            R"(tests/test_targets/xf-naca2408-il-50000.csv)",
    };

    const vector<tuple<char, size_t>> findsDelimAndLineIdxExpectedValues{
            {  // shortened_SEMS.dat
                    '\t', 3
            },
            {  // long_SEMS.dat
                    '\t', 54
            },
            {  // acsm_shortened.csv
                    ',',  0
            },
            {  // xf-naca2408-il-50000.csv
                    ',',  10
            },
    };

    vector<tuple<vector<string>, char, size_t>> findsDelimAndLineIdxTestValues;

    void SetUp() override {
        DelimTestFixture::findsDelimAndLineIdxTestValues = vector<tuple<vector<string>, char, size_t>>();
        populateFilesLines(fileTargets);

        size_t targetIdx = -1;
        for (const auto &target: this->fileTargets) {
            targetIdx++;
            auto expectedValues = DelimTestFixture::findsDelimAndLineIdxExpectedValues.at(targetIdx);
            DelimTestFixture::findsDelimAndLineIdxTestValues.emplace_back(
                    filesLines.at(targetIdx), get<0>(expectedValues), get<1>(expectedValues));
        }
    }
};


class FieldsTestFixture : public FileReaderHelperTestFixture {
protected:
    const vector<string> fileTargets{
            R"(tests/test_targets/shortened_SEMS.dat)",
    };

    vector<vector<vector<string>>> fieldsExpected{
            {  // shortened_sems.dat
                    {"#StartDate", "StartTime", "EndDate", "EndTime"},
                    {"181021", "16:02:53", "181021", "16:03:25"},
                    {"181021", "16:03:40", "181021", "16:04:10"},
                    {"181021", "16:04:25", "181021", "16:04:56"}
            },
    };

    void SetUp() override {
        populateFilesLines(fileTargets);
    }
};


class ClassificationTestFixture : public FileReaderHelperTestFixture {
protected:
    const vector<string> fileTargets{
            R"(tests/test_targets/shortened_SEMS.dat)",
            R"(tests/test_targets/acsm_shortened.csv)"
    };

    vector<vector<tuple<string, string>>> classificationsExpected{
            {  // shortened_sems.dat
                    {"#StartDate", "date"},
                    {"StartTime", "time"},
                    {"EndDate", "date"},
                    {"EndTime", "time"},
            },
            {
                    {"Org", "float_dec"},
                    {"SO4", "float_dec"},
                    {"NO3", "float_dec"},
                    {"NH4", "float_dec"},
                    {"Chl", "float_dec"},
                    {"pressureAlarm", "logical"},
                    {"airbeamAlarm", "logical"},
                    {"RF", "float_exp"},
                    {"chamberT", "float_dec"},
                    {"Airbeam", "float_exp"},
                    {"NewStart_Events", "logical"},
                    {"InletPClosed", "float_dec"},
                    {"InletPOpen", "float_dec"},
                    {"InletP", "float_dec"},
                    {"VapT", "float_dec"},
                    {"acsm_local_time", "datetime"},
                    {"acsm_utc_time", "datetime"}
            }
    };

    void SetUp() override {
        populateFilesLines(fileTargets);
    }
};

#endif //TEST_TABULATED_DATA_INFERENCE_H
