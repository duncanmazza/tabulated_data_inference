/**
 * Definitions for main library functions.
 *
 * @author Duncan Mazza
 */

#include <tabulated_data_inference.h>
#include <include/delim_helpers.h>


using namespace std;


MpcParserTWrapper::MpcParserTWrapper() {
    mpc_parser_t* parser;
    mpc_setup(&parser, _allParsers);
    _parser = parser;
}

mpc_parser_t *MpcParserTWrapper::getParserPtr() const {
    return _parser;
}

MpcParserTWrapper::~MpcParserTWrapper() {
    for (auto pIterator = _allParsers.rbegin(); pIterator != _allParsers.rend(); pIterator++) {
        mpc_undefine(*pIterator);
    }
    for (auto pIterator = _allParsers.rbegin(); pIterator != _allParsers.rend(); pIterator++) {
        mpc_delete(*pIterator);
    }
}


tuple<char, size_t> getDelim(const vector<string> &lines) {
#ifdef PRINT_FILE_CONTENTS
    for (const auto& line : lines) {
        cout << line << endl;
    }
#endif
    DelimFindingState state = DFS_FSM_NO_ENCOUNTERS;

    size_t consistencyCount[NDELIMS] = {0};
    size_t prevDelimCount[NDELIMS] = {0};
    size_t delimCount[NDELIMS + 1] = {0};  // Last element is throwaway

    size_t revLineIdx = lines.size();
    size_t lastNonemptyRevLineIdx = revLineIdx;
    for (auto revLineIterator = lines.rbegin(); revLineIterator != lines.rend(); revLineIterator++) {
        revLineIdx--;
        if (revLineIterator->empty()) {
            continue;
        }

        // Update prevDelimCount and zero out delimCount
        for (int i = 0; i < NDELIMS; i++) {
            prevDelimCount[i] = delimCount[i];
            delimCount[i] = 0;
        }

        for (auto charInLine: *revLineIterator) {
            delimCount[get_delim_idx(charInLine)] += 1;
        }

        state = delimFinderStateTrans(state, delimCount, prevDelimCount, consistencyCount);
        if (state == DFS_FSM_NO_CONSISTENCIES_LEFT) {
            break;
        }

        lastNonemptyRevLineIdx = revLineIdx;
    }

    // Either no consistencies are left or all lines have been iterated through; return the delimiter with the highest
    // consistency if at least one consistency > 0. Return null if there is a tie, if no consistencies were found, or
    // if no delimiters were found.
    if (state == DFS_FSM_NO_ENCOUNTERS) return {'\0', 0};

    size_t maxConsistency = 0;
    size_t maxConsistencyIdx;
    for (size_t i = 0; i < NDELIMS; i++) {
        if (consistencyCount[i] > maxConsistency) {
            maxConsistency = consistencyCount[i];
            maxConsistencyIdx = i;
        }
    }

    if (maxConsistency == 0) return {'\0', 0};

    // Check for a tie
    for (size_t i = 0; i < NDELIMS; i++) {
        if (i != maxConsistencyIdx && consistencyCount[i] == maxConsistency) return {'\0', 0};
    }

    return {DELIMS[maxConsistencyIdx], lastNonemptyRevLineIdx};
}


int getFields(const vector<string> &lines, char delim, vector<vector<string>> &ret, size_t stopAt) {
    if (lines.empty()) { return -1; }

    size_t numFieldsEncountered = 0;
    int consistentNumFields = -1;
    size_t lineIdx = lines.size();
    for (auto revLineIterator = lines.rbegin(); revLineIterator != lines.rend(); revLineIterator++) {
        lineIdx--;
        if (revLineIterator->empty()) {
            continue;
        }

        vector<string> splitLine;
        boost::split(splitLine, *revLineIterator, boost::is_any_of(string{delim}));

        if (consistentNumFields == -1) {
            numFieldsEncountered = splitLine.size();
            consistentNumFields = 1;
        } else {
            if (numFieldsEncountered != splitLine.size()) {
                consistentNumFields &= 0;
            }
        }

        ret.push_back(splitLine);

        if (lineIdx == stopAt) { break; }
    }
    reverse(ret.begin(), ret.end());
    return consistentNumFields;
}


FieldCls extractFieldClsFromParser(const mpc_result_t *const mpcResult, int mpcResultRet) {
    if (mpcResultRet) {
        string result = mpc_strip_tag(((mpc_ast_t *) mpcResult->output)->children[1]->tag);
        const char *resultCStr = result.c_str();
        for (int i = 0; i < NUM_FC - 1; i++) {
            if (!strcmp(resultCStr, FieldClsCorrespondingNames[i])) {
                return (FieldCls) i;
            }
        }
    }
    return FC_8_ARBITRY;
}


void classifyColumnsAutoSetup(const vector<vector<string>> &rows, vector<tuple<string, FieldCls>> &classifications) {
    if (rows.empty()) { return; }

    auto parser = MpcParserTWrapper();
    classifyColumns(rows, classifications, parser);
}


void classifyColumns(const vector<vector<string>> &rows, vector<tuple<string, FieldCls>> &classifications,
                     MpcParserTWrapper &parser) {
    mpc_result_t parseResult;
    int parseResultInt;
    size_t numLines = rows.size();
    size_t numFields = rows.at(0).size();
    size_t lineIdx = -1;
    size_t fieldIdx;

    // Start out by classifying all fields as arbitrary strings
    FieldCls fieldClasses[numFields];
    for (int i = 0; i < numFields; i++) {
        // Casting gets rid of const qualifiers
        fieldClasses[i] = FC_0_LOGICAL;
    }

    for (auto row = rows.rbegin(); row != rows.rend(); row++) {
        lineIdx++;

        // If at the top row, extract the header and populate classifications with it and the results of the data type
        // arbitration
        if (lineIdx == numLines - 1) {
            fieldIdx = -1;
            for (const auto &field: *row) {
                fieldIdx++;
                classifications.emplace_back(field, fieldClasses[fieldIdx]);
            }
            break;
        }

        // Iterate though the fields in this row and, for each one, find the most restrictive format that fits it
        fieldIdx = -1;
        for (auto const &field: *row) {
            fieldIdx++;
            parseResultInt = mpc_parse("input", field.c_str(), parser.getParserPtr(), &parseResult);
            auto resultEnum = extractFieldClsFromParser(&parseResult, parseResultInt);
            fieldClasses[fieldIdx] = std::max(fieldClasses[fieldIdx], resultEnum);
        }
    }
}
