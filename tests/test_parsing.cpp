/**
 * Unit tests for testing the parser combinator's grammar.
 *
 * @author Duncan Mazza
 */

#include <grammar.h>
#include "tabulated_data_inference.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>

//#define GRAMMAR_DEBUG

using namespace std;


TEST(GRAMMAR, GrammarParsing) {
    const vector<tuple<string, FieldCls>> grammar_test_targets{
            {
                    "0",                           FC_0_LOGICAL
            },
            {
                    "1",                           FC_0_LOGICAL
            },
            {
                    "0110",                        FC_1_BIT_STR
            },
            {  // %m-%d-%Y
                    "04-02-2022",                  FC_4_DT_ONLY
            },
            {  // %d%m%y
                    "181021",                      FC_4_DT_ONLY
            },
            {  // %m-%d-%y
                    "04-02-22",                    FC_4_DT_ONLY
            },
            {  // %Y-%m-%d (ISO extended)
                    "2022-04-02",                  FC_4_DT_ONLY
            },
            {  // %Y%m%d (ISO format, date only)
                    "20220402",                    FC_4_DT_ONLY
            },
            {  // extra character over %m-%d-%Y
                    "12-34-56789",                 FC_8_ARBITRY
            },
            {  // expect 12 hr time with PM
                    "13-34-2022 PM",               FC_8_ARBITRY
            },
            {  // %H:%M:%S%F %p
                    "10:03:22.0023 PM",            FC_3_TM_ONLY
            },
            {  // %H:%M:%S%F %p %ZP
                    "13:03:22.0023 PM MST-07",     FC_3_TM_ONLY
            },
            {  // %H%M%S%F%q (ISO format, time only)
                    "131211-0700",                 FC_3_TM_ONLY
            },
            {  // %Y-%b-%d %H:%M:%S%F %ZP (boost default input)
                    "2005-Oct-15 13:12:11 MST-07", FC_2_DT_TIME
            },
            {  // %Y%m%dT%H%M%S%F%q (ISO format)
                    "20051015T131211-0700",        FC_2_DT_TIME
            },
            {
                    "2/9/2022 19:16",              FC_2_DT_TIME
            },
            {
                    "2/9/2022 0:16",               FC_2_DT_TIME
            },
            {  // when faced with a string that can be either a date or time, choose date
                    "120402",                      FC_4_DT_ONLY
            },
            {
                    "12040",                       FC_5_INTEGER
            },
            {
                    ".123",                        FC_6_FLT_DEC
            },
            {
                    "0.123",                       FC_6_FLT_DEC
            },
            {
                    "30.123",                      FC_6_FLT_DEC
            },
            {
                    "4.63E-11",                    FC_7_FLT_EXP
            },
            {
                    "-7e+02",                      FC_7_FLT_EXP
            },
            {
                    "abcd123",                     FC_8_ARBITRY
            }
    };

    auto parser = MpcParserTWrapper();

    mpc_result_t r;
    int result;

    for (auto target: grammar_test_targets) {
        string input_str = get<0>(target);
        FieldCls expected_type = get<1>(target);

        result = mpc_parse("input", input_str.c_str(), parser.getParserPtr(), &r);

        if (expected_type != FC_8_ARBITRY) {  // expected mpc not to fail
            if (!result) {
                cout << "Error with " << input_str << endl;
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            } else {
#ifdef GRAMMAR_DEBUG
                cout << "ast for '" << input_str << "': " << endl;
                mpc_ast_print((mpc_ast_t *) r.output);
                cout << endl << endl;
#endif
            }
            ASSERT_TRUE(result);

            auto resultEnum = extractFieldClsFromParser(&r, result);
            ASSERT_EQ(resultEnum, expected_type);
        } else {
            if (result) {
                cout << "Expected parsing error on input: " << input_str << endl;

#ifdef GRAMMAR_DEBUG
                cout << "ast for '" << input_str << "': " << endl;
                mpc_ast_print((mpc_ast_t *) r.output);
                cout << endl << endl;
#endif
            }

            ASSERT_FALSE(result);
        }
    }
}
