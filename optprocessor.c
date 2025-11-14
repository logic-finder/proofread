#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "fatal.h"
#include "wrapper.h"
#include "readline.h"
#include "optprocessor.h"

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
#define DIRSEP "/"
#else
#define DIRSEP "\\"
#endif

#ifndef PATH
#define PATH "bin"
#endif

// `handle_hlpopt` (help option) prints the help manual.
static void handle_hlpopt(void);
// `handle_vopt` (version option) prints the version.
static void handle_vopt(bool extflg);
// `handle_vvopt` (-vv option) prints the detailed information on this program.
static void handle_vvopt(void);
// `handle_vvvopt` (-vvv option) prints something.
static void handle_vvvopt(void);
// `validate_opts` (validate options) checkes whether the program is invoked with appropriate arguments.
static void validate_opts(optflg_t *of, dynarr_t *filenames);
// `init_opts` (initialize options) gives some options default values.
static void init_opts(optflg_t *of);

extern void process_opts(optflg_t *of, dynarr_t *filenames) {
   if (of->hlp)
      handle_hlpopt();
   else
      switch (of->vsn) {
         case 0: break;
         case 1: handle_vopt(true); break;
         case 2: handle_vvopt(); break;
         case 3: handle_vvvopt(); break;
      }
   validate_opts(of, filenames);
   init_opts(of);
}

static void print_manual(void);

static void handle_hlpopt(void) {
   #if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
      int ret;

      /*
      system(NULL) RETURN VALUE
      0 = shell is not available
      non-zero = no problem
      */
      ret = system(NULL);
      if (!ret) print_manual();
      /*
      MAN(1) EXIT STATUS
      0 = no problem
      1|2|3|16 = bad
      */
      ret = system("man " PATH DIRSEP "dat" DIRSEP "proofread.1");
      if (ret) print_manual();  /* falls back on .txt */
      exit(EXIT_SUCCESS);
   #else
      print_manual();
   #endif
}

static void print_manual(void) {
   FILE *fp;
   char *line;

   fp = sfopen(PATH DIRSEP "dat" DIRSEP "proofread.1.txt", "r");
   while (!readln(fp, &line)) {
      sfputs(stdout, line);
      free(line);
   }
   sfclose(fp);
   exit(EXIT_SUCCESS);
}

static void handle_vopt(bool extflg) {
   FILE *fp;
   char *line;

   fp = sfopen(PATH DIRSEP "dat" DIRSEP "version.txt", "r");
   if (!readln(fp, &line)) {
      fmtwrt("proofread %s", line);
      free(line);
   }
   sfclose(fp);

   if (extflg) exit(EXIT_SUCCESS);
}

static void handle_vvopt(void) {
   handle_vopt(false);
   fmtwrt("\nPATH = %s\n", PATH);
   sputs("\nDeveloped by Doohyeon Won (logicseeker@naver.com)");
   sputs("For more information, please visit https://github.com/logic-finder/proofread.");
   exit(EXIT_SUCCESS);
}

#if 0
static char *ceasar(const char *msg, int shift);
#endif

static void handle_vvvopt(void) {
   /* HINT: the number of ! */
   sputs("tpiewi qevvc qi esfe edywe ym erh rexwy gler!!!!");
   exit(EXIT_SUCCESS);
}

#if 0
static char *ceasar(const char *msg, int shift) {
   int i, len;
   char ch, *ret;

   len = strlen(msg);
   ret = smalloc(len);

   for (i = 0; i < len; i++) {
      ch = msg[i];
      if (islower(ch))
         ret[i] = ((ch - 'a') + shift) % 26 + 'a';
      else
      if (isupper(ch))
         ret[i] = ((ch - 'A') + shift) % 26 + 'A';
      else
         ret[i] = ch;
   }
   ret[len] = '\0';

   return ret;
}
#endif

static void validate_opts(optflg_t *of, dynarr_t *filenames) {
   int len;

   len = dynarr_len(filenames);
   if (of->owf && len == 0)
      fatal("proofread: no files provided after +.");
}

static void init_opts(optflg_t *of) {
   if (!of->lne) of->ful = true;
   if (!of->owf) of->sto = true;
}
