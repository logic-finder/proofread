#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fatal.h"
#include "strutil.h"
#include "wrapper.h"
#include "argparse.h"
#include "readline.h"
#include "optprocessor.h"
#include "colorcode.h"

/**
 * @brief `proofread` is the main logic of this program, hence the name.
 * @param of the result of argument parsing
 * @param filenames an array of filenames
 * @param keeppath the value of `--keep` option
 */
static void proofread(optflg_t *of, dynarr_t *filenames, const char *keeppath);

int main(int argc, const char *argv[]) {
   optflg_t *of;
   dynarr_t *filenames;
   const char *keeppath;  /* --keep=path */

   of = scalloc(1, sizeof *of);
   filenames = parse_args(of, argv, &keeppath);
   process_opts(of, filenames);
   proofread(of, filenames, keeppath);

   free(of);
   free(filenames);
   return 0;
}

// `run_dry` gets an invocation when `--dry-run` is specified.
static void run_dry(optflg_t *of, dynarr_t *filenames);
// `run_actual` gets an invocation when it performs real tasks.
static void run_actual(optflg_t *of, dynarr_t *filenames, const char *keeppath);

static void proofread(optflg_t *of, dynarr_t *filenames, const char *keeppath) {
   if (of->drn)
      run_dry(of, filenames);
   else
      run_actual(of, filenames, keeppath);
}

// `apply_dryrun` performs a dry-run action on a file based on `of`.
static void apply_dryrun(FILE *fp, optflg_t *of);

static void run_dry(optflg_t *of, dynarr_t *filenames) {
   char *eolstr = of->eol ? "-l " : "";
   char *eofstr = of->eof ? "-f " : "";
   char *substr = of->lne ? "line " : "full ";
   char *modstr = of->owf ? "+ -- " : "- ";

   // handles + option
   if (of->owf) {
      int idx, len;

      len = dynarr_len(filenames);
      for (idx = 0; idx < len; idx++) {
         char *filename;
         FILE *fp;

         // gets a stream
         filename = *((char **) dynarr_get(filenames, idx));
         fp = sfopen(filename, "r");

         // echoes the enabled options
         if (idx > 0) sputs("");
         fmtwrt("%s> proofread %s%s--dry-run=%s%s%s%s\n",
            Cyellow, eolstr, eofstr, substr, modstr, filename, Creset);
         sputs("=================================================");

         // main logic
         apply_dryrun(fp, of);

         // cleanup
         sfclose(fp);
      }
   }
   // handles the default option
   else {
      // echoes the enabled options
      fmtwrt("%s> proofread %s%s--dry-run=%s%s%s\n",
         Cyellow, eolstr, eofstr, substr, modstr, Creset);
      sputs("=================================================");

      // main logic
      apply_dryrun(stdin, of);
   }
}

// `handle_eoldrn` checkes the eol problem in a dry-run.
static int handle_eoldrn(char *line, int numline, bool dryrun_line);
// `handle_eofdrn` checkes the eof problem in a dry-run.
static bool handle_eofdrn(int numline, int lastmod, bool dryrun_line, bool newline_at_eol);

static void apply_dryrun(FILE *fp, optflg_t *of) {
   int numline, lret, fret, lastmod;
   bool newline_at_eol;
   char *line, *crnt, *prev;

   numline = 1;   /* current line number */
   lret = 0;  /* once become 1, remain 1. */
   lastmod = 0;   /* last modified line */
   crnt = NULL;

   // reads a line from fp and prints it
   while (!readln(fp, &line)) {
      newline_at_eol = lastch(line) == '\n';

      // with -l enabled, highlights whitespaces at eol
      if (of->eol) {
         int rv = handle_eoldrn(line, numline, of->lne);
         lret |= rv;
         if (rv) lastmod = numline;
      }
      else
      // otherwise, just prints the line untouched
      if (!of->lne)
         fmtwrt("%4d|%s", numline, line);

      prev = crnt;
      crnt = line;
      free(prev);
      numline++;
   }

   // crnt != NULL test is necessary because the file can be empty.
   if (crnt) {
      // with -f enabled, checks if there is a missing \n at eof
      // newline_at_eol must hold a value since crnt != NULL.
      if (of->eof) {
         fret = handle_eofdrn(numline, lastmod, of->lne, newline_at_eol);
         if (!lret && !fret)
            sputs("(Nothing to fix)");
      }
      else
      if (of->ful) {
         if (newline_at_eol)
            fmtwrt("%4d|\n", numline);
         else
            sputs("\n(No newline before EOF)");
      }
      else {   /* neither eof nor ful */
         if (!lret) sputs("(Nothing to fix)");
         else
         if (!newline_at_eol && lastmod == numline - 1)
            sputs("");
      }
   }
   else
   // empty file
   if (numline == 1 && !crnt)
      sputs("(EOF)");

   // do usual cleanup
   free(crnt);
}

// `cntspn` (count span) counts how many unnecessary spaces are at eol.
static int cntspn(char *line);
// `cntdgt` (count digit) counts how many digits are in `num`.
static unsigned int cntdgt(unsigned int num);

static int handle_eoldrn(char *line, int numline, bool dryrun_line) {
   unsigned int len, spn, cnt, ret;

   // calculates the amount of whitespace at eol
   len = strlen(line);
   spn = cntspn(line);
   cnt = cntdgt(spn);
   ret = 0;

   // whitespace being present at eol
   if (spn) {
      int nloc, nspace;
      bool cond;

      nloc = len - spn;
      cond = lastch(line) == '\n';
      if (cond) nloc--;
      line[nloc] = '\0';

      fmtwrt("%s%4d|%s%s%s",
         Cyellow, numline, Creset, line, CBred);

      nspace = spn - cnt;

      if (spn == 1)
         fmtwrt("1%s", Creset);
      else
         fmtwrt("%*c%d%s", nspace, ' ', spn, Creset);

      if (cond) sputs("");

      ret = 1;
   }
   else
   // no whitespace at eol, but needs to print the line unchanged
   if (!dryrun_line)
      fmtwrt("%4d|%s", numline, line);

   return ret;
}

static int cntspn(char *line) {
   int i, len, span;

   len = strlen(line);
   span = 0;

   i = len - 1;
   if (lastch(line) == '\n')
      i--;

   for (/* blank */; i >= 0; i--)
      if (match(line[i], " \f\t\v"))
         span++;
      else
         break;

   return span;
}

static unsigned int cntdgt(unsigned int num) {
   char buf[11]; /* 4,294,967,295 */
   int cnt;

   cnt = sprintf(buf, "%d", num);
   if (cnt < 0) exit(EXIT_FAILURE);

   return cnt;
}

static bool handle_eofdrn(int numline, int lastmod, bool dryrun_line, bool newline_at_eol) {
   bool ret = true;

   if (!newline_at_eol) {
      if (!dryrun_line || lastmod == numline - 1)
         sputs("");
      fmtwrt("%s%4d|%s%s\\n%s\n", Cyellow, numline, Creset, CBgreen, Creset);
   }
   else
   // already having \n at eof, just needs to print the line number
   if (!dryrun_line)
      fmtwrt("%4d|\n", numline);
   else
      ret = false;

   return ret;
}

// `apply_actual` performs a real task on a file based on `of`.
static void apply_actual(FILE *src, FILE *dest, optflg_t *of);
// `cleanup` removes and/or renames the original and result file.
static void cleanup(optflg_t *of, const char *srcname, const char *destname, const char *keeppath);

static void run_actual(optflg_t *of, dynarr_t *filenames, const char *keeppath) {
   // handles + option
   if (of->owf) {
      int idx, len;

      len = dynarr_len(filenames);
      for (idx = 0; idx < len; idx++) {
         char *srcname, *destname;
         FILE *src, *dest;

         // gets a stream
         srcname = *((char **) dynarr_get(filenames, idx));
         destname = concat(2, srcname, ".prfrd.tmp");
         src  = sfopen(srcname, "r");
         dest = sfopen(destname, "w");

         // main logic
         apply_actual(src, dest, of);

         // cleanup
         sfclose(src);
         sfclose(dest);
         cleanup(of, srcname, destname, keeppath);
         free(destname);
      }
   }
   // handles the default option
   else apply_actual(stdin, stdout, of);
}

// `handle_eolact` gives the eol-problem an actual fix.
static void handle_eolact(char *line, FILE *dest);
// `handle_eofact` gives the eof-problem an actual fix.
static void handle_eofact(FILE *dest, bool newline_at_eol);

static void apply_actual(FILE *src, FILE *dest, optflg_t *of) {
   char *line, *crnt, *prev;
   bool newline_at_eol;

   // reads a line from stdin and prints it
   crnt = NULL;

   while (!readln(src, &line)) {
      newline_at_eol = lastch(line) == '\n';

      // with -l enabled, removes whitespaces at eol
      if (of->eol)
         handle_eolact(line, dest);
      else
      // otherwise, just prints the line untouched
         sfputs(dest, line);
      prev = crnt;
      crnt = line;
      free(prev);
   }

   // with -f enabled, puts \n at eof, if not existed
   // crnt != NULL test is necessary because the file can be empty.
   if (of->eof && crnt)
      // newline_at_eol must hold a value since crnt != NULL.
      handle_eofact(dest, newline_at_eol);

   // do usual cleanup
   free(crnt);
}

static void handle_eolact(char *line, FILE *dest) {
   int len, span;
   bool cond;

   // calculates the amount of whitespace at eol
   len = strlen(line);
   span = cntspn(line);

   // whitespace being present at eol
   if (span) {
      cond = lastch(line) == '\n';
      if (cond) span++;
      line[len - span] = '\0';
      sfputs(dest, line);
      if (cond) sfputc(dest, '\n');
   }
   else
   // no whitespace at eol, but needs to print the line unchanged
      sfputs(dest, line);
}

static void handle_eofact(FILE *dest, bool newline_at_eol) {
   if (!newline_at_eol)
      sfputc(dest, '\n');
}

static void cleanup(optflg_t *of, const char *srcname, const char *destname, const char *keeppath) {
   #if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
      char *dirsep = "/";
   #else
      char *dirsep = "\\";
   #endif
   char *rnmmsg = "%s is renamed to %s.\n";

   if (!of->kep) {
      /*
      srcfile  | input.txt -> (deleted)
      destfile | input.txt.prfrd.tmp -> input.txt
      */
      sremove(srcname);
      if (!of->mut) fmtwrt("%s is deleted.\n", srcname);
      srename(destname, srcname);
      if (!of->mut) fmtwrt(rnmmsg, destname, srcname);
      return;
   }
   else {
      /*
      srcfile  | d/input.txt -> keeppath/input.txt
      destfile | d/input.txt.prfrd.tmp -> d/input.txt
      */
      char *onlyname, *newname;

      /*
      No need to test onlyname == NULL since if so,
      the file wouldn't have opened in the first place.
      */
      onlyname = extfnm(srcname);
      newname = concat(3, keeppath, dirsep, onlyname);

      if (rename(srcname, newname)) {
         sremove(destname);
         vfatal("unable to move %s to %s; "
            "maybe the given directory \"%s\" does not exist?",
            srcname, newname, keeppath);
      }
      if (!of->mut) fmtwrt(rnmmsg, srcname, newname);
      srename(destname, srcname);
      if (!of->mut) fmtwrt(rnmmsg, destname, srcname);
      free(newname);
      free(onlyname);
   }
}
