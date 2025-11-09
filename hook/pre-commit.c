#include <stdio.h>
#include <string.h>
#include "fatal.h"
#include "strutil.h"
#include "wrapper.h"
#include "readline.h"

#ifndef EXT_LIST
#define EXT_LIST  /* if not specified, all A/M files are processed. */
#endif

#ifndef SHUTUP
#define SHUTUP 0
#endif

#define Cgreen  "\033[0;32m"
#define Cred    "\033[0;31m"
#define Creset  "\033[0m"

static void show_ext_list(int len, char *list[*]);
static bool has_valid_ext(void *_ext, int _, void *arr);
static void modify_worktree(char *filename);
static void modify_index(char *filename);

char *from = "abtnvfr";
char *to = "\a\b\t\n\v\f\r";

char *msg_cmd = "pre-commit: failed to construct a command to execute.\n";
char *msg_prf = "pre-commit: proofread exited with a non-zero status.\n";
char *msg_git = "pre-commit: git exited abnormally.\n";

int main(int argc, const char **argv) {
   char *ext[] = {   /* the extensions to accept. */
      NULL, /* ext[0] has a special meaning: it's meant to be a filename. */
      EXT_LIST
   };
   int extlen = sizeof ext / sizeof ext[0] - 1;

   // prints the list of valid extensions, if given the "l" option
   if (argc > 1 && !strcmp(argv[1], "l")) {
      show_ext_list(extlen, ext + 1);
      return 2;
   }

   // checks whether a shell is available or not
   int ret;

   ret = system(NULL);
   if (!ret) fatal("pre-commit: unable to use a command processor.");

   // The hook begins.
   sputs("pre-commit: the hook executed.");

   /*
   - stores the output of `git status --short` to a temporary file
   - The file consists of one or more lines, since it's a pre-commit hook.
   - Each line looks like this:
      line  MM argparse.c
      idx   0123...
   A = added
   M = modified
   - Refer to https://git-scm.com/docs/git-status#_short_format.
   */
   FILE *stshrt;
   char *stshrt_name = ".prfrd.short-status";

   stshrt = sfopen(stshrt_name, "w+");
   ret = system("git status --short > .prfrd.short-status");
   if (ret != 0) fatal("pre-commit: failed to execute 'git status'.");

   /*
   - The following while loop reads a line from the tempfile
   until there are no more lines.

   - If the line satisfies one of the conditions below,
   process the line appropriately:

      <condition 1> [AM] [ ] filename
      <condition 2> [AM] [M] filename

   In case of cond. 1, this hook executes these commands:

      $ proofread -lf --mute + -- "filename"
      $ git add -- "filename"

   In other words, it modifies the worktree and update the index.

   In case of cond. 2, this hook executes these commands:

      $ git checkout-index --temp -- "filename"
      $ proofread -lf --mute + -- "tempfilename"
      $ git hash-object -w "tempfilename"
      $ git update-index --add --cacheinfo 100644,<object>,"filename"

   That is, it only updates the index. The reason is that it can't
   update the worktree since there are local modifications.

   Incidentally, filenames get quoted considering its potential
   having whitespaces in it.

   - Otherwise, the loop skips to the next line.
   */
   char *s;

   // prints out the ext. list
   show_ext_list(extlen, ext + 1);

   // if the file is empty, nothing happens.
   while (!readln(stshrt, &s)) {
      char *line;
      int cond;

      // going to do some pointer arithmetic on line, not s
      // since we need to free 's' later.
      line = s;

      if (!match(line[0], "AM"))
         goto cleanup;   /* only applies to this statuses */
      if (line[1] == ' ')
         cond = 1;   /* worktree is clean (no local changes) */
      else
      if (line[1] == 'M')
         cond = 2;   /* index differs with worktree */
      else
         goto cleanup;

      line += 3;  /* where a filename begins */

      // removes a newline at EOL
      line[strlen(line) - 1] = '\0';

      // removes double-quotes
      if (line[0] == '"') {
         line += 1;
         line[strlen(line) - 1] = '\0';

         // the line may have escape characters.
         char *ret;

         ret = escape(line, '\\', from, to);
         free(s);
         s = line = ret;
      }

      // checks whether the filename contains a valid extension
      ext[0] = line;
      ret = exist(ext + 1, sizeof (char *), extlen, has_valid_ext);
      if (ret == -1) continue;

      // does the job depending on the condition
      if (cond == 1)
         modify_worktree(line);
      else
         modify_index(line);

      cleanup: free(s);
   }

   // cleanup
   sfclose(stshrt);
   sremove(stshrt_name);

   // end of hook
   sputs("pre-commit: the hook finished.");
   return 0;   /* exits with 0 */
}

static void show_ext_list(int len, char *list[len]) {
   sfputs(stdout, "pre-commit: target extension:");
   if (!len)
      sfputs(stdout, " none");
   else for (int i = 0; i < len; i++)
      fmtwrt(" %s", list[i]);
   sfputc(stdout, '\n');
}

static bool has_valid_ext(void *_ext, int _, void *arr) {
   char *ext, *line;

   ext = *((char **) _ext);
   line = ((char **) arr)[-1];  /* recall the `exist` call. */

   return endwth(line, ext) ? true : false;
}

static void modify_worktree(char *filename) {
   char *normal;

   normal = normalize(filename, '\\', to, from);
   fmtwrt("pre-commit: %s'%s'%s has no local change; modifying both the index and the worktree.\n", Cgreen, normal, Creset);

   char *cmdbase[] = {
      "proofread -lf --mute + --",
      "git add --"
   };

   int ret, cmdlen, fnlen;
   char *cmd;

   fnlen = strlen(filename);
   cmdlen = strlen(cmdbase[0]) + 1 + 1 + fnlen + 1;
   cmd = smalloc(cmdlen + 1);
   ret = sprintf(cmd, "%s '%s'", cmdbase[0], filename);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s '%s'\n", cmdbase[0], normal);
   ret = system(cmd);
   if (ret != EXIT_SUCCESS) fatal(msg_prf);

   // cmd can accommodate cmdbase[1] since it's shorter than [0].
   cmdlen = strlen(cmdbase[1]) + 1 + 1 + fnlen + 1;
   ret = sprintf(cmd, "%s '%s'", cmdbase[1], filename);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s '%s'\n", cmdbase[1], normal);
   ret = system(cmd);
   if (ret != 0) fatal(msg_git);

   free(cmd);
   free(normal);
}

static void modify_index(char *filename) {
   char *normal;

   normal = normalize(filename, '\\', to, from);
   fmtwrt("pre-commit: %s'%s'%s has local changes; modifying the index only.\n", Cred, normal, Creset);

   char *cmdbase[] = {
      "git checkout-index --temp --",
      "proofread -lf --mute + --",
      "git hash-object -w --",
      "git update-index --add --cacheinfo 100644,"
   };
   char *chkidx_name = ".prfrd.checkout-index-temp";
   FILE *chkidx = sfopen(chkidx_name, "w+");

   int ret, cmdlen, fnlen;
   char *cmd;

   fnlen = strlen(filename);
   cmdlen = strlen(cmdbase[0]) + 1 + 1 + fnlen + 1 + 3 + strlen(chkidx_name);
   cmd = smalloc(cmdlen + 1);
   ret = sprintf(cmd, "%s '%s' > %s", cmdbase[0], filename, chkidx_name);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s '%s' > %s\n", cmdbase[0], normal, chkidx_name);
   ret = system(cmd);
   if (ret != 0) fatal(msg_git);
   free(cmd);

   char *idxtmp_name;
   int i, idxtmp_len;

   ret = readln(chkidx, &idxtmp_name);
   if (ret) vfatal("pre-commit: failed to read %s.\n", chkidx_name);

   // refer to https://git-scm.com/docs/git-checkout-index#_using_temp_or_stageall
   idxtmp_len = strlen(idxtmp_name);

   for (i = 0; i < idxtmp_len; i++)
      if (idxtmp_name[i] == '\t')
         break;
   if (i == idxtmp_len) fatal("pre-commit: the tempfile has a wrong content.");
   idxtmp_name[i] = '\0';
   idxtmp_len = strlen(idxtmp_name);

   cmdlen = strlen(cmdbase[1]) + 1 + 1 + idxtmp_len + 1;
   cmd = smalloc(cmdlen + 1);
   ret = sprintf(cmd, "%s '%s'", cmdbase[1], idxtmp_name);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s '%s'\n", cmdbase[1], idxtmp_name);
   ret = system(cmd);
   if (ret != 0) fatal(msg_prf);
   free(cmd);

   char *hshobj_name = ".prfrd.hash-object-temp";
   FILE *hshobj = sfopen(hshobj_name, "w+");

   cmdlen = strlen(cmdbase[2]) + 1 + 1 + idxtmp_len + 1 + 3 + strlen(hshobj_name);
   cmd = smalloc(cmdlen + 1);
   ret = sprintf(cmd, "%s '%s' > %s", cmdbase[2], idxtmp_name, hshobj_name);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s '%s' > %s\n", cmdbase[2], idxtmp_name, hshobj_name);
   ret = system(cmd);
   if (ret != 0) fatal(msg_git);
   free(cmd);

   char *hshval;
   int hshval_len;

   ret = readln(hshobj, &hshval);
   if (ret) vfatal("pre-commit: failed to read %s.\n", hshobj_name);

   hshval_len = strlen(hshval);
   hshval[hshval_len - 1] = '\0';  /* removes \n */

   cmdlen = strlen(cmdbase[3]) + 40 + 1 + 1 + fnlen + 1;
   cmd = smalloc(cmdlen + 1);
   ret = sprintf(cmd, "%s%s,'%s'", cmdbase[3], hshval, filename);
   if (ret != cmdlen) fatal(msg_cmd);
   if (!SHUTUP) fmtwrt("pre-commit: %s%s,'%s'\n", cmdbase[3], hshval, filename);
   ret = system(cmd);
   if (ret != 0) fatal(msg_git);
   free(cmd);

   sfclose(chkidx);
   sfclose(hshobj);
   sremove(chkidx_name);
   sremove(idxtmp_name);
   sremove(hshobj_name);
   free(idxtmp_name);
   free(hshval);
   free(normal);
}
