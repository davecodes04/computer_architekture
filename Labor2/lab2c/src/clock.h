#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdlib.h>

#define ERROR(str, err)                        \
    do                                         \
    {                                          \
        fprintf(stderr, "ERROR (%s:%d): %s\n", \
                __FILE__, __LINE__, (str));    \
        exit(err);                             \
    } while (0)

/* The type for a (x, y) position, each between 0 and screen-width/-height */
typedef struct {
    int x;
    int y;
} pos_t;

/* The type for a box definition upper-left and lower-right corner */
typedef struct {
    pos_t upper_left;
    pos_t lower_right;
} box_t;

#endif /* __CLOCK_H__ */