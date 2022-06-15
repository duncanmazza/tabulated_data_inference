/**
 * Definitions for the helper functions used for finding the delimiter of a file.
 *
 * @author Duncan Mazza
 */

#include <delim_helpers.h>


DelimFindingState
delimFinderStateTrans(const DelimFindingState currState, const size_t *const currDelims, const size_t *const prevDelims,
                      size_t *const consistencyCount) {
    switch (currState) {
        case DFS_FSM_NO_ENCOUNTERS:
            for (int i = 0; i < NDELIMS; i++) {
                if (currDelims[i] != 0) return DFS_FSM_ONE_OR_MORE_CONSISTENCIES_FOUND;
            }
            return DFS_FSM_NO_ENCOUNTERS;
        case DFS_FSM_ONE_OR_MORE_CONSISTENCIES_FOUND:
            break;
        case DFS_FSM_NO_CONSISTENCIES_LEFT:
            return DFS_FSM_NO_CONSISTENCIES_LEFT;
    }

    // ONE_OR_MORE_CONSISTENCIES_FOUND
    int at_least_one_consistency = 0;
    for (int i = 0; i < NDELIMS; i++) {
        if (currDelims[i] != 0 && prevDelims[i] == currDelims[i]) {
            consistencyCount[i] += 1;
            at_least_one_consistency |= 1;
        }
    }

    if (!at_least_one_consistency) return DFS_FSM_NO_CONSISTENCIES_LEFT;
    else return DFS_FSM_ONE_OR_MORE_CONSISTENCIES_FOUND;
}


int get_delim_idx(char delim) {
    if (DELIMS[0] == delim) return 0;
    else if (DELIMS[1] == delim) return 1;
    else if (DELIMS[2] == delim) return 2;
    else if (DELIMS[3] == delim) return 3;
    else
        return 4;
}
