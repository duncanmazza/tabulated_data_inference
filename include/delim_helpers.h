/**
 * Headers for the helper functions used for finding the delimiter of a file.
 *
 * @author Duncan Mazza
 */

#ifndef DELIMITED_FILE_INFERENCE_DELIM_HELPERS_H
#define DELIMITED_FILE_INFERENCE_DELIM_HELPERS_H

#include <cstdlib>


typedef enum {
    DFS_FSM_NO_ENCOUNTERS,
    DFS_FSM_ONE_OR_MORE_CONSISTENCIES_FOUND,
    DFS_FSM_NO_CONSISTENCIES_LEFT
} DelimFindingState;


const size_t NDELIMS = 4;
const char DELIMS[]{',', ';', ' ', '\t'};

DelimFindingState delimFinderStateTrans(DelimFindingState currState, const size_t *currDelims,
                                        const size_t *prevDelims, size_t *consistencyCount);

int get_delim_idx(char delim);

#endif //DELIMITED_FILE_INFERENCE_DELIM_HELPERS_H
