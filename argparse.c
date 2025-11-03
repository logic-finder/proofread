#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fatal.h"
#include "strutil.h"
#include "wrapper.h"
#include "readline.h"
#include "argparse.h"

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
#define DIRSEP "/"   /* directory separator */
#else
#define DIRSEP "\\"
#endif

static char *warnmsg =
   "Note: in case of this being a filename, "
   "please specify '--' first, the end of option marker.\n"
   "Note: type -h or --help to get help.";

// `parse_shrtop` (short option) processes options starting with `-`.
static void parse_shrtop(const char *arg, optflg_t *of);
// `parse_longop` (long option) processes options starting with `--`.
static void parse_longop(const char *arg, optflg_t *of, const char **keeppath, dynarr_t *files);
// `parse_wrtmod` (writing mode) processes `-` or `+` option.
static void parse_wrtmod(const char *arg, optflg_t *of);
// `parse_endopt` (end-of-option marker) processes `--`.
static void parse_endopt(optflg_t *of);
// `parse_filenm` (filename) processes filenames.
static void parse_filenm(const char *arg, optflg_t *of, dynarr_t *files);

extern dynarr_t *parse_args(optflg_t *of, const char *argv[], const char **keeppath) {
   /*
   Synopsis:
      (1) proofread [-l] [-f] [--dry-run[=(full|line)]] [-]
      (2) proofread [-l] [-f] [--dry-run[=(full|line)]] [--mute] + [--] <file>...
      (3) proofread [-l] [-f] [--keep[=<path>]] [--mute] + [--] <file>...
      (4) proofread (--clean=<file> | --smudge)
      (5) proofread (-h|--help)
      (6) proofread (-v|-vv|-vvv)
   */
   dynarr_t *files = dynarr_create(sizeof (char *));

   /*
   The loop starts at i = 1, since argv[0] is the program's name;
   it terminates when argv[i] is NULL.
   */
   int i;
   const char *arg;

   for (i = 1; (arg = argv[i]); i++) {
      /* After the end of option marker, treats EVERYTHING as filename. */
      if (of->eoo) {
         dynarr_append(files, &arg);
         continue;
      }

      // detects -x options
      if (strlen(arg) > 1 && strspn(arg, "-") == 1)
         parse_shrtop(arg, of);
      else
      // detects --x options
      if (strlen(arg) > 2 && strspn(arg, "-") == 2)
         parse_longop(arg, of, keeppath, files);
      else
      // detects - or +
      if (strlen(arg) == 1 && match(arg[0], "-+"))
         parse_wrtmod(arg, of);
      else
      // detects --, or the end of option marker
      if (strlen(arg) == 2 && !strcmp(arg, "--"))
         parse_endopt(of);
      else
      // handles unrecognizable arguments
      if (arg[0] == '-')
         vfatal("unable to recognize this option: %s.\n%s", arg, warnmsg);
      else
      // takes the arg as a filename
         parse_filenm(arg, of, files);
   }

   return files;
}

// `validate_hlpopt` (help option) checks whether `-h` is valid in the context it appears.
static void validate_hlpopt(optflg_t *of);

/*
FIXME: the following codes have this shape:

if (arg equals to opt_1) {
   // tests to see if it is fine to take this arg
   // sets the option
}
else if (arg equals to opt_2) { ... }

It seems it is possible to refactor like so:

opts = [opt_1, opt_2, ...]
handlers = [opt_1_handler, opt_2_handler, ...]

for (until the end of args) {
   // checks whether arg is in opts; if exists, gets the index
   // invokes the corresponding handler: handlers[index]()
}

void opt_n_handler(optflg_t *of) {
   // tests to see if it is fine to take this arg
   // sets the option
}
*/

static void parse_shrtop(const char *arg, optflg_t *of) {
   int i;
   char ch;

   for (i = 1; (ch = arg[i]); i++) {
      if (ch == 'l') {
         if (of->eol) fatal("-l seen already.");
         if (of->hlp) fatal("-h with -l.");
         if (of->vsn) fatal("-v with -l.");
         if (of->cln) fatal("--clean with -l.");
         if (of->smg) fatal("--smudge with -l.");
         of->eol = true;
      }
      else
      if (ch == 'f') {
         if (of->eof) fatal("-f seen already.");
         if (of->hlp) fatal("-h with -f.");
         if (of->vsn) fatal("-v with -f.");
         if (of->cln) fatal("--clean with -f.");
         if (of->smg) fatal("--smudge with -f.");
         of->eof = true;
      }
      else
      if (ch == 'h') {
         validate_hlpopt(of);
         of->hlp = true;
      }
      else
      if (ch == 'v') {
         if (of->eol) fatal("-l with -v.");
         if (of->eof) fatal("-f with -v.");
         if (of->hlp) fatal("-h with -v.");
         if (of->drn) fatal("--dry-run with -v.");
         if (of->sto) fatal("- with -v.");
         if (of->owf) fatal("+ with -v.");
         if (of->vsn == 3)
            fatal("-v specified more than three times.");
         if (of->kep) fatal("--keep with -v.");
         if (of->mut) fatal("--mute with -v.");
         if (of->cln) fatal("--clean with -v.");
         if (of->smg) fatal("--smudge with -v.");
         of->vsn++;
      }
      else
      // handles unrecognizable arguments
         vfatal("unable to recognize this option: -%c.\n%s", ch, warnmsg);
   }  /* end of loop */
}

static void validate_hlpopt(optflg_t *of) {
   if (of->eol) fatal("-l with -h.");
   if (of->eof) fatal("-f with -h.");
   if (of->hlp) fatal("-h seen already.");
   if (of->vsn) fatal("-v with -h.");
   if (of->drn) fatal("--dry-run with -h.");
   if (of->sto) fatal("- with -h.");
   if (of->owf) fatal("+ with -h.");
   if (of->kep) fatal("--keep with -h.");
   if (of->mut) fatal("--mute with -h.");
   if (of->cln) fatal("--clean with -h.");
   if (of->smg) fatal("--smudge with -h.");
}

// `validate_clnopt` (clean option) checks whether `--clean` is valid in the context it appears.
static void validate_clnopt(optflg_t *of);
// `validate_smgopt` (smudge option) checks whether `--smudge` is valid in the context it appears.
static void validate_smgopt(optflg_t *of);

static void parse_longop(const char *arg, optflg_t *of, const char **keeppath, dynarr_t *files) {
   // skips --
   arg += 2;

   // detects --dry-run
   if (!strncmp(arg, "dry-run", 7)) {
      if (of->hlp) fatal("-h with --dry-run.");
      if (of->vsn) fatal("-v with --dry-run.");
      if (of->drn) fatal("--dry-run seen already.");
      if (of->kep) fatal("--keep with --dry-run.");
      if (of->cln) fatal("--clean with --dry-run.");
      if (of->smg) fatal("--smudge with --dry-run.");

      arg += 7;
      if (strlen(arg) > 0) {
         if (!strcmp(arg, "=full"))
            of->ful = true;
         else
         if (!strcmp(arg, "=line"))
            of->lne = true;
         else
            vfatal("unable to recognize the suboption: %s.\n%s", arg, warnmsg);
      }

      of->drn = true;
   }
   else
   // detects --help
   if (!strncmp(arg, "help", 4)) {
      validate_hlpopt(of);
      of->hlp = true;
   }
   else
   // detects --keep
   if (!strncmp(arg, "keep", 4)) {
      if (of->hlp) fatal("-h with --keep.");
      if (of->vsn) fatal("-v with --keep.");
      if (of->drn) fatal("--dry-run with --keep.");
      if (of->sto) fatal("- with --keep.");
      if (of->kep) fatal("--keep seen already.");
      if (of->cln) fatal("--clean with --keep.");
      if (of->smg) fatal("--smudge with --keep.");

      arg += 4;
      if (strlen(arg) > 0) {
         if (arg[0] != '=')
            fatal("--keep has an incorrect syntax.");
         *keeppath = arg + 1;
      }
      else
         *keeppath = "bak";

      of->kep = true;
   }
   else
   // detects --mute
   if (!strncmp(arg, "mute", 4)) {
      if (of->hlp) fatal("-h with --mute.");
      if (of->vsn) fatal("-v with --mute.");
      if (of->sto) fatal("- with --mute.");
      if (of->cln) fatal("--clean with --mute.");
      if (of->smg) fatal("--smudge with --mute.");
      of->mut = true;
   }
   else
   // detects --clean
   if (!strncmp(arg, "clean", 5)) {
      validate_clnopt(of);

      arg += 5;
      if (strlen(arg) > 0) {
         if (arg[0] != '=')
            fatal("--clean has an incorrect syntax.");
         arg += 1;
         dynarr_append(files, &arg);
      }
      else fatal("--clean requires a <file>.");

      of->cln = true;
   }
   else
   // detects --smudge
   if (!strncmp(arg, "smudge", 6)) {
      validate_smgopt(of);
      of->smg = true;
   }
   else
   // handles unrecognizable arguments
      vfatal("unable to recognize this option: --%s.\n%s", arg, warnmsg);
}

// `validate_clnsmg` (clean & smudge) checks conditions that --clean and --smudge needs to pass in common.
static void validate_clnsmg(optflg_t *of, const char *opt);

static void validate_clnopt(optflg_t *of) {
   validate_clnsmg(of, "--clean");
   if (of->cln) fatal("--clean seen already.");
   if (of->smg) fatal("--smudge with --clean.");
}

static void validate_smgopt(optflg_t *of) {
   validate_clnsmg(of, "--smudge");
   if (of->cln) fatal("--clean with --smudge.");
   if (of->smg) fatal("--smudge seen already.");
}

static void validate_clnsmg(optflg_t *of, const char *opt) {
   if (of->eol) vfatal("-l with %s.", opt);
   if (of->eof) vfatal("-f with %s.", opt);
   if (of->hlp) vfatal("-h with %s.", opt);
   if (of->vsn) vfatal("-v with %s.", opt);
   if (of->drn) vfatal("--dry-run with %s.", opt);
   if (of->sto) vfatal("- with %s.", opt);
   if (of->owf) vfatal("+ with %s.", opt);
   if (of->kep) vfatal("--keep with %s.", opt);
   if (of->mut) vfatal("--mute with %s.", opt);
}

static void parse_wrtmod(const char *arg, optflg_t *of) {
   if (arg[0] == '-') {
      if (of->hlp) fatal("-h with -.");
      if (of->vsn) fatal("-v with -.");
      if (of->kep) fatal("--keep with -.");
      if (of->mut) fatal("--mute with -.");
      if (of->cln) fatal("--clean with -.");
      if (of->smg) fatal("--smudge with -.");
      of->sto = true;
   }
   else {  /* '+' */
      if (of->hlp) fatal("-h with +.");
      if (of->vsn) fatal("-v with +.");
      if (of->cln) fatal("--clean with +.");
      if (of->smg) fatal("--smudge with +.");
      of->owf = true;
   }
}

static void parse_endopt(optflg_t *of) {
   if (of->hlp) fatal("-h with --.");
   if (of->vsn) fatal("-h with --.");
   if (of->cln) fatal("--clean with --.");
   if (of->smg) fatal("--smudge with --.");
   of->eoo = true;
}

void parse_filenm(const char *arg, optflg_t *of, dynarr_t *files) {
   if (!of->owf) fatal("+ option must be present before a filename.");
   dynarr_append(files, &arg);
}
