#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "strutil.h"
#include "wrapper.h"

extern bool match(char ch, const char *scanset) {
   char cmp;
   bool ret;

   ret = false;
   while ((cmp = *scanset++)) {
      if (ch != cmp)
         continue;
      ret = true;
      break;
   }

   return ret;
}

extern char *concat(int n, const char *base, ...) {
   va_list ap;
   int i, va_cnt, len;
   char *s, *ret;

   va_cnt = n - 1; /* the number of variable arguments */
   len = strlen(base);

   va_start(ap, base);
   for (i = 0; i < va_cnt; i++) {
      s = va_arg(ap, char *);
      len += strlen(s);
   }

   ret = smalloc(len + 1);
   strcpy(ret, base);

   va_start(ap, base);
   for (i = 0; i < va_cnt; i++) {
      s = va_arg(ap, char *);
      strcat(ret, s);
   }

   va_end(ap);
   return ret;
}

extern char *extfnm(const char *path) {
   #if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
   #define DIRSEP '/'
   #else
   #define DIRSEP '\\'
   #endif

   int i, len, pos;
   char *filename;

   len = strlen(path);
   pos = -1;

   /*
   needs to consider the following four situations:
      /s, s, /, and \0
   */

   for (i = len - 1; i >= 0; i--)
      if (path[i] == DIRSEP) {
         pos = i;
         break;
      }

   /* handles "/" and "" */
   if (path[pos + 1] == '\0')  /* path does not have a filename */
      return NULL;

   filename = smalloc(len);

   /* handles "s" */
   if (pos == -1)   /* path does not have a dirsep */
      strcpy(filename, path);
   else
   /* handles "/s" */
      strcpy(filename, &path[pos + 1]);

   return filename;
}

extern bool endwth(char *target, char *against) {
   int tlen, alen;

   tlen = strlen(target);
   alen = strlen(against);

   if (alen > tlen)
      return false;

   // it is true that pos >= 0.
   int pos = tlen - alen;

   return !strcmp(&target[pos], against) ? true : false;
}

extern int exist(void *arr, int esiz, int len, bool (*cb)(void *, int, void *)) {
   if (len <= 0)
      return -2;

   int i;

   for (i = 0; i < len; i++)
      if (cb((char *) arr + i * esiz, i, arr))
         return i;

   return -1;
}

extern char *escape(const char *str, char escaper, const char *from, const char *to) {
   char *buf;
   int p, q;
   ptrdiff_t i;

   buf = smalloc(strlen(str) + 1);
   p = q = 0;
   while (str[p] != '\0') {
      if (str[p] == escaper && match(str[p + 1], from)) {
         i = strchr(from, str[p + 1]) - from;
         buf[q++] = to[i];
         p += 2;
      }
      else buf[q++] = str[p++];
   }
   buf[q] = '\0';

   return buf;
}

// inline function
extern char lastch(const char *line);
