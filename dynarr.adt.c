#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "dynarr.adt.h"

#define DYNARR_INIT_LIMIT 4

struct dynarr {
   unsigned int limit;
   unsigned int len;
   unsigned int elemsiz;
   void *arr;
};

// `dynarr_expand` enlarges the array.
static void dynarr_expand(dynarr_t *dynarr);
// `dynarr_calcaddr` (calculate address) returns the address of `idx`th element.
static void *dynarr_calcaddr(dynarr_t *dynarr, unsigned int idx);

extern dynarr_t *dynarr_create(unsigned int elemsiz) {
   dynarr_t *dynarr;

   dynarr = smalloc(sizeof *dynarr);

   dynarr->arr = smalloc(elemsiz * DYNARR_INIT_LIMIT);
   dynarr->limit = DYNARR_INIT_LIMIT;
   dynarr->len = 0;
   dynarr->elemsiz = elemsiz;

   return dynarr;
}

extern void dynarr_set(dynarr_t *dynarr, unsigned int idx, void *elem) {
   void *dest;

   if (dynarr->len == dynarr->limit)
      dynarr_expand(dynarr);

   dest = dynarr_calcaddr(dynarr, idx);
   memcpy(dest, elem, dynarr->elemsiz);
}

extern void dynarr_append(dynarr_t *dynarr, void *elem) {
   dynarr_set(dynarr, dynarr->len, elem);
   dynarr->len++;
}

extern void *dynarr_get(dynarr_t *dynarr, unsigned int idx) {
   return dynarr_calcaddr(dynarr, idx);
}

extern unsigned int dynarr_len(dynarr_t *dynarr) {
   return dynarr->len;
}

static void dynarr_expand(dynarr_t *dynarr) {
   dynarr->limit *= 2;
   dynarr->arr = srealloc(dynarr->arr, dynarr->limit);
}

static void *dynarr_calcaddr(dynarr_t *dynarr, unsigned int idx) {
   int offset = idx * dynarr->elemsiz;
   return (char *) dynarr->arr + offset;
}
