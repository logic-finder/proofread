#ifndef STRUTIL_H
#define STRUTIL_H

#include <stdarg.h>
#include <stdbool.h>

/**
 * @brief `match` returns true if `ch` exists in `scanset`.
 * @param ch a character
 * @param scanset a null-terminated string
 * @return true or false
 */
bool match(char ch, const char *scanset);

/**
 * @brief `concat` appends a variable number of strings onto `base` in the listed order.
 * @param n the number of strings to concatenate including `base`.
 * @param base a string
 * @return a null terminated string
 * @note The return value needs to be freed later.
 */
char *concat(int n, const char *base, ...);

/**
 * @brief `extfnm` (extract filename) removes the directory portion from `path`,
 *    leaving only the filename.
 * @param path a path including a filename.
 * @return a filename
 * @note The return value needs to be freed later.
 */
char *extfnm(const char *path);

/**
 * @brief `lastch` returns the last character in `line`, hence the name.
 * @param line a string
 * @return a last character
 */
inline char lastch(const char *line) {
   return line[strlen(line) - 1];
}

#endif
