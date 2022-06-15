/**
 * Headers for the helper functions used with the parser combinator.
 *
 * @author Duncan Mazza
 */

#ifndef DELIMITED_FILE_INFERENCE_GRAMMAR_H
#define DELIMITED_FILE_INFERENCE_GRAMMAR_H

extern "C" {
#include <mpc.h>
}
#include <regex>

using namespace std;


void mpc_setup(mpc_parser_t **parser, vector<mpc_parser_t *> &allParsers);

string mpc_strip_tag(const string &tag);

#endif //DELIMITED_FILE_INFERENCE_GRAMMAR_H
