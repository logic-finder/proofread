#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>

#define READLINE_UNIT   64

/**
 * @brief `readln` (read line) reads a line from `fp`. Notable behaviors:
 *    (1) it stores `\n` and `\0`.
 *    (2) it returns 0 in success; returns 1 in failure (EOF).
 *    (3) it overwrites `\r` with `\n`.
 * @param fp a stream
 * @param line a variable to store the line
 * @return a status code
 * @note The return value needs to be freed later.
 */
int readln(FILE *fp, char **line);

#endif
