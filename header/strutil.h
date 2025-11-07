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
 * @brief `endwth` (ends with) determines whether `target` ends with `against`.
 *    For instance, the call `endwth("argparse.c", ".c")` returns `true`.
 * @param target a string to compare against
 * @param against a string to compare with
 * @return a Boolean value, i.e. `true` or `false`.
 */
bool endwth(char *target, char *against);

/**
 * @brief `exist` returns the index of the first element which is in `arr`
 *    and passes the test by `cb`.
 * @param arr the array containing element(s)
 * @param esiz the size of an element
 * @param len the length of the array
 * @param cb a function which returns a Boolean value depending on
 *    whether an element passes the test in this function
 * @param cb.arg[0]=elem the pointer to the element
 * @param cb.arg[1]=idx the index of the element
 * @param cb.arg[2]=arr the address of the first element in the `arr`
 * @return If len <= 0, returns -2. If no element has passed the test,
 *    returns -1. Otherwise, it returns the index of the first element
 *    which has passed the test.
 */
int exist(void *arr, int esiz, int len, bool (*cb)(void *elem, int idx, void *arr));

/**
 * @brief `escape` resolves all of the escaped characters in the string `str`.
 * @param str a string
 * @param escaper a escape character
 * @param from a string consisting of characters to be resolved
 * @param to a string consisting of characters which the escaped characters to become
 * @return a resolved string
 * @note The return value needs to be freed later.
 */
char *escape(const char *str, char escaper, const char *from, const char *to);

/**
 * @brief `normalize` resolves specific characters represented by `from` into respective characters that are represented by `to` and that are prefixed by a character `escaper`.
 * @param str a string
 * @param escaper a escape character
 * @param from a string consisting of characters to be resolved
 * @param to a string consisting of characters which the target characters to become
 * @return a normalized string
 * @note The return value needs to be freed later.
 */
char *normalize(const char *str, char escaper, const char *from, const char *to);

/**
 * @brief `lastch` returns the last character in `line`, hence the name.
 * @param line a string
 * @return a last character
 */
inline char lastch(const char *line) {
   return line[strlen(line) - 1];
}

#endif
