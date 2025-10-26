#ifndef FATAL_H
#define FATAL_H

#include <stdarg.h>

/**
 * @brief `fatal` prints an error message `msg` to `stderr`.
 *    The string "fatal: " is prepended.
 * @param msg an error message
 */
void fatal(const char *msg);

/**
 * @brief `vfatal` prints a format string `msg` to `stderr`.
 *    The string "fatal: " is prepended.
 * @param msg a format string
 * @param va variable number of arguments
 */
void vfatal(const char *msg, ...);

#endif
