#ifndef DYNARR_ADT_H
#define DYNARR_ADT_H

/*
`dynarr_t` is a kind of list type which contains elements of the same type.
This array has no limit in size, hence comes the name dynamic array.

`dynarr_t` is an abstract data type;
its actual implementation is in `dynarr.adt.c`.
*/
typedef struct dynarr dynarr_t;

/**
 * @brief `dynarr_create` makes a new `dynarr_t` object.
 * @param elemsiz the size of one element
 * @return a pointer to a `dynarr_t` object
 * @note The return value needs to be freed later.
 */
dynarr_t *dynarr_create(unsigned int elemsiz);

/**
 * @brief `dynarr_set` sets an element at index `idx`.
 * @param dynarr the array
 * @param idx the index
 * @param elem the value to put
 * @note The value is copied into the array.
 */
void dynarr_set(dynarr_t *dynarr, unsigned int idx, void *elem);

/**
 * @brief `dynarr_append` puts an element at the tail.
 * @param dynarr the array
 * @param elem the value to put
 * @note The value is copied into the array.
 */
void dynarr_append(dynarr_t *dynarr, void *elem);

/**
 * @brief `dynarr_get` peeks an element at index `idx`.
 * @param dynarr the array
 * @param idx the index
 * @return the pointer to the element
 */
void *dynarr_get(dynarr_t *dynarr, unsigned int idx);

/**
 * @brief `dynarr_len` returns the number of elements.
 * @param dynarr the array
 * @return the length of it
 */
unsigned int dynarr_len(dynarr_t *dynarr);

/**
 * @brief `dynarr_destroy` frees the object.
 * @param dynarr the array
 */
void dynarr_destroy(dynarr_t *dynarr);

#endif
