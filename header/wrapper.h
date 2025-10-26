#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief `smalloc` (safe malloc) is a wrapper for `malloc`.
 * @note exits in failure.
 */
void *smalloc(size_t siz);

/**
 * @brief `srealloc` (safe realloc) is a wrapper for `realloc`.
 * @note exits in failure.
 */
void *srealloc(void *ptr, size_t siz);

/**
 * @brief `scalloc` (safe calloc) is a wrapper for `calloc`.
 * @note exits in failure.
 */
void *scalloc(size_t n, size_t siz);

/**
 * @brief `sfopen` (safe fopen) is a wrapper for `fopen`.
 * @note exits in failure.
 */
FILE *sfopen(const char *filename, const char *mode);

/**
 * @brief `sfclose` (safe fclose) is a wrapper for `fclose`.
 * @note exits in failure.
 */
void sfclose(FILE *fp);

/**
 * @brief `sremove` (safe remove) is a wrapper for `remove`.
 * @note exits in failure.
 */
void sremove(const char *filename);

/**
 * @brief `srename` (safe rename) is a wrapper for `rename`.
 * @note exits in failure.
 */
void srename(const char *old, const char *new);

/**
 * @brief `sfputs` (safe fputs) is a wrapper for `fputs`.
 * @note exits in failure.
 */
void sfputs(FILE *stream, const char *line);

/**
 * @brief `sputs` (safe puts) is a wrapper for `puts`.
 * @note exits in failure.
 */
void sputs(const char *line);

/**
 * @brief `sfputc` (safe putc) is a wrapper for `putc`.
 * @note exits in failure.
 */
void sfputc(FILE *stream, char c);

/**
 * @brief `ffmtwrt` (file format write) is a wrapper for `fprintf`.
 * @note exits in failure.
 */
void ffmtwrt(FILE *stream, const char *format, ...);

/**
 * @brief `fmtwrt` (format write) is a wrapper for `printf`.
 * @note exits in failure.
 */
void fmtwrt(const char *format, ...);

// FILE *stmpfile(void);

#endif
