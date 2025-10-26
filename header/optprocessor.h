#ifndef OPTPROCESSER_H
#define OPTPROCESSER_H

#include "argparse.h"
#include "dynarr.adt.h"

/**
 * @brief `process_opts` (process options) does three tasks:
 *    (1) handles `-h` or `-v` option, if exists.
 *    (2) validate the parsed options
 *    (3) initialize default options
 * @param of an option flag object
 * @param filenames an array of filenames
 */
void process_opts(optflg_t *of, dynarr_t *filenames);

#endif
