#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void panic(char *m)
{
	printf("%s\n", m);
	exit(1);
}

#define BUG() do { \
        printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __FUNCTION__); \
        panic("BUG!"); \
} while (0)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof(((type *) 0)->member) *__mptr = (ptr);     \
        (type *) ((char *) __mptr - offsetof(type, member));})

static inline void *xmalloc(size_t size)
{
        void *p = NULL;

        if (!(p = malloc(size)))
                panic("ERROR: out of memory\n");

        return p;
}

static inline void *xzalloc(size_t size)
{
        void *ptr = xmalloc(size);
        memset(ptr, 0, size);
        return ptr;
}

int memory_display(const void *addr, loff_t offs, unsigned nbytes, int size, int swab);

#endif /* __COMMON_H_ */
