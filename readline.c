#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "strutil.h"
#include "wrapper.h"
#include "readline.h"

extern int readln(FILE *fp, char **line) {
   // tests whether there is nothing to read
   int ch;

   ch = getc(fp);
   if (ch == EOF) {
      if (ferror(fp)) exit(EXIT_FAILURE);
      *line = NULL;
      return 1;
   }
   else ungetc(ch, fp);

   // reads chars from the file so as to construct a string
   int bufsiz = READLINE_UNIT;
   char *buf = smalloc(bufsiz);
   int pos;
   bool eol;

   pos = 0;
   eol = false;

   for (;;) {
      if (eol) {
         buf[pos] = '\0';
         *line = buf;
         return 0;
      }

      ch = getc(fp);

      if (ch == EOF) {
         if (ferror(fp)) exit(EXIT_FAILURE);
         buf[pos] = '\0';
         *line = buf;
         return 0;
      }
      if (ch == '\r')
         continue;
      if (ch == '\n')
         eol = true;

      buf[pos++] = ch;

      if (pos == bufsiz) {
         bufsiz *= 2;
         buf = srealloc(buf, bufsiz);
      }
   }
}
