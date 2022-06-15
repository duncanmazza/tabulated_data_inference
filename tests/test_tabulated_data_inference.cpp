/**
 * Unit tests for functions defined in delimited_file_inference.h.
 *
 * @author Duncan Mazza
 */

#include <test_tabulated_data_inference.h>

using namespace std;


TEST_F(DelimTestFixture, FindsDelimAndLineIdx) {
    for (auto testCase: findsDelimAndLineIdxTestValues) {
        auto getDelimRet = getDelim(get<0>(testCase));
        ASSERT_EQ(get<0>(getDelimRet), get<1>(testCase));
        ASSERT_EQ(get<1>(getDelimRet), get<2>(testCase));
    }
}


TEST_F(FieldsTestFixture, FindsFields) {
    size_t fileIdx = -1;
    for (const auto &thisFileLines: filesLines) {
        fileIdx++;
        auto delimRet = getDelim(thisFileLines);

        vector<vector<string>> fieldRet;
        int consistentFields = getFields(thisFileLines, get<0>(delimRet), fieldRet, get<1>(delimRet));
        ASSERT_TRUE(consistentFields);

        vector<vector<string>> expectedFieldsThisFile = fieldsExpected.at(fileIdx);
        ASSERT_TRUE(expectedFieldsThisFile.size() == fieldRet.size());

        size_t lineIdx = -1;
        for (const auto &line: fieldRet) {
            lineIdx++;
            size_t fieldIdx = -1;
            for (const auto &field: line) {
                fieldIdx++;
                ASSERT_EQ(field, expectedFieldsThisFile.at(lineIdx).at(fieldIdx));
            }
        }
    }
}


TEST_F(ClassificationTestFixture, ClassifiesFile) {
    size_t fileIdx = -1;
    auto parser = MpcParserTWrapper();

    for (const auto &thisFileLines: filesLines) {
        fileIdx++;
        auto delimRet = getDelim(thisFileLines);
        vector<vector<string>> fieldRet;
        int consistentFields = getFields(thisFileLines, get<0>(delimRet), fieldRet, get<1>(delimRet));
        ASSERT_TRUE(consistentFields);

        vector<tuple<string, FieldCls>> classificationRet;
        classifyColumns(fieldRet, classificationRet, parser);
        vector<tuple<string, string>> expectedClassificationsThisFile = classificationsExpected.at(fileIdx);
        ASSERT_TRUE(expectedClassificationsThisFile.size() == classificationRet.size());

        size_t classificationIdx = -1;
        for (const auto &classification: classificationRet) {
            classificationIdx++;
            auto thisFieldName = get<0>(classification).c_str();
            ASSERT_STREQ(get<0>(expectedClassificationsThisFile.at(classificationIdx)).c_str(),
                         thisFieldName);

            auto thisFieldExpectedCls = get<1>(expectedClassificationsThisFile.at(classificationIdx)).c_str();
            auto thisFieldActualCls = FieldClsCorrespondingNames[get<1>(classification)];
            if (strcmp(thisFieldExpectedCls, thisFieldActualCls) != 0) {
                cout << "Incorrect classification for " << thisFieldName << ":" << endl;
            }
            ASSERT_STREQ(thisFieldExpectedCls, thisFieldActualCls);
        }
    }
}
