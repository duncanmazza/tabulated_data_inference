/**
 * Main interface for the library.
 *
 * @author Duncan Mazza
 */

#ifndef TABULATED_DATA_INFERENCE_H
#define TABULATED_DATA_INFERENCE_H

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <grammar.h>

using namespace std;

typedef enum {
    FC_0_LOGICAL,  // boolean values
    FC_1_BIT_STR,  // bit string
    FC_2_DT_TIME,  // datetime string
    FC_3_TM_ONLY,  // time string
    FC_4_DT_ONLY,  // date string
    FC_5_INTEGER,  // integer
    FC_6_FLT_DEC,  // floating point (decimal notation)
    FC_7_FLT_EXP,  // floating point (scientific notation)
    FC_8_ARBITRY,  // arbitrary string
} FieldCls;
const int NUM_FC = 9;

const char *const FieldClsCorrespondingNames[]{
        "logical",
        "bit_str",
        "datetime",
        "time",
        "date",
        "int",
        "float_dec",
        "float_exp",
        "arbitrary",
};


/**
 * Wrapper for creating and cleaning up mpc_parser_t pointers.
 */
class MpcParserTWrapper {
private:
    mpc_parser_t* _parser;
    vector<mpc_parser_t*> _allParsers;
public:
    MpcParserTWrapper();
    virtual ~MpcParserTWrapper();
    mpc_parser_t *getParserPtr() const;
};


/**
 * Infer the delimiter of data
 *
 * @note Assume that the delimiter is one of the following: comma, semicolon, space, or (horizontal) tab.
 * @note This function works by identifying which of the possible delimiter characters remains the most consistent in
 *  occurrences-per-line starting from the end of the file. Empty lines are ignored.
 *
 * @param lines Vector of strings where each string is a line in the data file
 * @return Tuple containing the delimiter found and the index of the last non-empty line identified as consistently
 *  using the delimiter. If a delimiter could not be successfully inferred, then the '\0' character is returned as the
 *  delimiter.
 */
tuple<char, size_t> getDelim(const vector<string> &lines);

/**
 * Given a vector of strings and a delimiter, acquire each of the fields in each column as a vector of vector of
 * strings.
 *
 * @param lines Vector of strings where each string is a line in the data file
 * @param ret Vector to which each line's fields are appended as a vector
 * @param delim Data delimiter
 * @param stopAt Stop acquiring fields from the data when this number + 1 of non-empty lines is reached (iteration happens
 * from the end of `lines`; this value can be thought of as the index of nonempty lines that reverse iteration stops at
 * when acquiring fields).
 * @return 1 if a consistent number of fields was found in every non-empty line and 0 if not.
 */
int getFields(const vector<string> &lines, char delim, vector<vector<string>> &ret, size_t stopAt = -1);


/**
 * A utility function for extracting the `FieldCls` enumeration value from the result given by mpc parsing.
 *
 * @note It is assumed that the string of the second child of the root node of the abstract syntax tree corresponds to
 * one of the strings in `FieldClsCorrespondingNames`, which in turn correspond to values in the `FieldCls` enumeration.
 * If there is no matching string in `FieldClsCorrespondingNames`, or if the `mpcResultRet` argument is 0, then the
 * default return value is `FC_8_ARBITRY`.
 *
 * @param mpcResult Result from parsing a string with the mpc parser combinator.
 * @param mpcResultRet The integer returned by `mpc_parse`.
 * @return The extracted `FieldCls` enumeration value.
 */
FieldCls extractFieldClsFromParser(const mpc_result_t *mpcResult, int mpcResultRet);


/**
 * Wrapper for the `classifyColumns` function that automatically creates (and discards) a parser object.
 *
 * @param rows A vector of at least length 1 where each element corresponds to a row of data. The first row is
 *  expected to contain the column names.
 * @param classifications A vector of tuples where each tuple contains as its first entry the column name and its second
 *  entry the column classification.
 */
void classifyColumnsAutoSetup(const vector<vector<string>> &rows, vector<tuple<string, FieldCls>> &classifications);


/**
 * Given columns of data (represented as a vector of rows where each row is a vector of strings), identify the least-
 * restrictive data classification for each column.
 *
 * @note The `FieldCls` enumeration enumerates the data classifications from most to least restrictive.
 * @note It is the caller's responsibility to manage the memory for the parser.
 *
 * @param rows A vector of at least length 1 where each element corresponds to a row of data. The first row is
 *  expected to contain the column names.
 * @param classifications A vector of tuples where each tuple contains as its first entry the column name and its second
 *  entry the column classification.
 * @param parser An object containing the mpc parser with which each string of data is parsed.
 */
void classifyColumns(const vector<vector<string>> &rows, vector<tuple<string, FieldCls>> &classifications,
                     MpcParserTWrapper &parser);


#endif //TABULATED_DATA_INFERENCE_H
