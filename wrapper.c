#include "fatal.h"
#include "wrapper.h"

/************
 * stdlib.h *
 ************/
extern void *smalloc(size_t siz) {
   void *ret = malloc(siz);
   if (!ret) fatal("proofread: malloc error.");
   return ret;
}

extern void *srealloc(void *ptr, size_t siz) {
   ptr = realloc(ptr, siz);
   if (!ptr) fatal("proofread: realloc error.");
   return ptr;
}

extern void *scalloc(size_t n, size_t siz) {
   void *ret = calloc(n, siz);
   if (!ret) fatal("proofread: calloc error.");
   return ret;
}

/***********
 * stdio.h *
 ***********/
extern FILE *sfopen(const char *filename, const char *mode) {
   FILE *fp = fopen(filename, mode);
   if (!fp) vfatal("unable to open the file: %s.", filename);
   return fp;
}

extern void sfclose(FILE *fp) {
   if (fclose(fp) != EOF)
      return;
   fatal("proofread: unable to close a stream.");
}

#if 0
extern FILE *stmpfile(void) {
   FILE *fp = tmpfile();
   if (!fp) fatal("proofread: unable to open a temporary file.");
   return fp;
}
#endif

extern void sremove(const char *filename) {
   if (!remove(filename))
      return;
   vfatal("proofread: unable to remove the file: %s.", filename);
}

extern void srename(const char *old, const char *new) {
   if (!rename(old, new))
      return;
   vfatal("proofread: unable to rename %s to %s.", old, new);
}

extern void sfputs(FILE *stream, const char *line) {
   if (fputs(line, stream) == EOF)
      fatal("proofread: fputs error.");
}

extern void sputs(const char *line) {
   if (puts(line) == EOF)
      fatal("proofread: puts error.");
}

extern void sfputc(FILE *stream, char c) {
   if (putc(c, stream) == EOF)
      fatal("proofread: fputc error.");
}

extern void ffmtwrt(FILE *stream, const char *format, ...) {
   va_list ap;
   int ret;

   va_start(ap, format);
   ret = vfprintf(stream, format, ap);
   if (ret < 0) exit(EXIT_FAILURE);
   va_end(ap);
}

extern void fmtwrt(const char *format, ...) {
   va_list ap;
   int ret;

   va_start(ap, format);
   ret = vprintf(format, ap);
   if (ret < 0) exit(EXIT_FAILURE);
   va_end(ap);
}
