#ifndef ARGPARSE_H
#define ARGPARSE_H

#include "dynarr.adt.h"

/*
option flags (bit-field)
   1 => true or false
   2 => 0, 1, 2, 3
*/
typedef struct optflg {
   unsigned int
      eol: 1,   // -l
      eof: 1,   // -f
      hlp: 1,   // -h
      vsn: 2,   // -v
      drn: 1,   // --dry-run
      ful: 1,   // full
      lne: 1,   // line
      sto: 1,   // -
      owf: 1,   // +
      eoo: 1,   // --
      kep: 1,   // --keep
      mut: 1,   // --mute
      cln: 1,   // --clean
      smg: 1;   // --smudge
} optflg_t;

/**
 * @brief `parse_args` takes command-line arguments and inspects them.
 *    It fills `of` based on the parsing result.
 * @param of the option flag struct
 * @param argv argument vector
 * @param keeppath a variable to store the value of `--keep` option
 */
dynarr_t *parse_args(optflg_t *of, const char *argv[], const char **keeppath);

#endif
